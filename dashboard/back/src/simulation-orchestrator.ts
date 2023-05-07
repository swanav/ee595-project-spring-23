import { ChildProcessWithoutNullStreams, spawn } from 'child_process'
import { EventEmitter } from 'ws'
import { getExperiment } from './experiment'

export type SimulationParameters = {
    algorithm: string,
    flows: number,
    packet_error_rate: number,
    bottleneck_bandwidth: string,
    bottleneck_delay: string,
    data_to_transmit_kb: number,
    mtu: number,
    duration: number,
    seed: number,
}

export type CongestionWindowDataPoint = {
    time: number,
    cwnd: number,
}

export type SimulationResult = {
    parameters: SimulationParameters,
    results: {
        throughput: number,
        packet_loss: number,
    },
    data: {
        throughput: number[],
        cwnd_progression: CongestionWindowDataPoint[][]
    }
}

export default class SimulationOrchestrator extends EventEmitter {

    private ns3Process: ChildProcessWithoutNullStreams | null = null
    private rlAgentProcess: ChildProcessWithoutNullStreams | null = null
    private experiment_id: string

    private algorithms: string[] = ['TcpNewReno', 'TcpCubic', 'TcpVegas', 'TcpMl', ]
    private seed: number = 1
    private iterations: number = 10
    private numberOfFlows: number = 1
    private mtu: number = 1500
    private data: number = 100

    private db: any

    constructor(db: any, experiment_id: string, timeout: number = 1000) {
        console.log('SimulationOrchestrator constructor')
        super()
        this.experiment_id = experiment_id
        this.db = db
        setTimeout(() => {
            this.emit('status', 'completed')
            this.emit('completed')
        }, timeout * 1000)
    }


    public async startSimulation() {
        console.log('SimulationOrchestrator startSimulation')

        const experiment = await getExperiment(this.db, this.experiment_id)
        console.log(experiment)


        const results: SimulationResult[] = []

        for (const algorithm of this.algorithms) {
            const params: SimulationParameters = {
                algorithm,
                flows: this.numberOfFlows,
                packet_error_rate: 0,
                bottleneck_bandwidth: '10Mbps',
                bottleneck_delay: '10ms',
                data_to_transmit_kb: this.data,
                mtu: this.mtu,
                duration: 10,
                seed: this.seed,
            }

            if (algorithm === 'TcpMl') {
                this.iterations
            }

            try {
                const result = await this.runSimulation(params)
                this.emit('result', result)
                results.push(result)
            } catch (error) {
                console.log(error)
            }

        }
        this.emit('results', results)
    }

    public async stopSimulation() {
        console.log('SimulationOrchestrator stopSimulation')
        this.ns3Process?.kill()
        this.rlAgentProcess?.kill()
    }

    private parseCongestionWindowProgression(input: string): CongestionWindowDataPoint[][] {
        if (input === undefined || input.length === 0) {
            return [];
        }
        // Split the input into lines and remove any leading/trailing whitespace
        const lines = input.split('\n').map(line => line.trim());
        // Get all lines that start with "Node:"
        const nodesData = lines.filter(line => line.startsWith('Node'))
            .map(line => line.trim());

        // For all the nodesData, extract the data points
        const dataPoints = nodesData.map(nodeData => {
            return nodeData.split('-')[1].split(',')
                .map(point => point.trim())
                .filter(point => point.length > 0)
                .map((point): CongestionWindowDataPoint => {
                    const match = point.match(/\(([\d.]+):([\d.]+)\)/);
                    if (!match) {
                        throw new Error(`Failed to parse congestion window progression data point: ${point}`);
                    }
                    const [, stime, scwnd] = match;
                    const time = parseFloat(stime);
                    const cwnd = parseInt(scwnd, 10);
                    return { time, cwnd };
                });
        });
        return dataPoints;
    }

    private parseSimulationParameters(data: string): SimulationParameters {
        const result: SimulationParameters = {
            algorithm: '',
            flows: 0,
            packet_error_rate: 0,
            bottleneck_bandwidth: '',
            bottleneck_delay: '',
            data_to_transmit_kb: 0,
            mtu: 0,
            duration: 0,
            seed: 0,
        };

        // Parsing Simulation Parameters
        const paramSection = data.match(/Execution Parameters:(.*)Congestion Window Progression:/s);
        if (paramSection) {
            const [, params] = paramSection;
            const algorithmMatch = params.match(/Congestion Control Protocol: (\S+)/);
            if (algorithmMatch) {
                result.algorithm = algorithmMatch[1];
            }

            const flowsMatch = params.match(/Number of flows: (\d+)/);
            if (flowsMatch) {
                result.flows = parseInt(flowsMatch[1], 10);
            }

            const packetErrorRateMatch = params.match(/Packet error rate: ([\d.]+)/);
            if (packetErrorRateMatch) {
                result.packet_error_rate = parseFloat(packetErrorRateMatch[1]);
            }

            const bottleneckBandwidthMatch = params.match(/Bottleneck bandwidth: (\S+)/);
            if (bottleneckBandwidthMatch) {
                result.bottleneck_bandwidth = bottleneckBandwidthMatch[1];
            }

            const bottleneckDelayMatch = params.match(/Bottleneck delay: (\S+)/);
            if (bottleneckDelayMatch) {
                result.bottleneck_delay = bottleneckDelayMatch[1];
            }

            const dataToTransmitMatch = params.match(/Data to transmit: (\d+) KB/);
            if (dataToTransmitMatch) {
                result.data_to_transmit_kb = parseInt(dataToTransmitMatch[1], 10);
            }

            const mtuMatch = params.match(/MTU: (\d+) bytes/);
            if (mtuMatch) {
                result.mtu = parseInt(mtuMatch[1], 10);
            }

            const durationMatch = params.match(/Duration: (\d+) seconds/);
            if (durationMatch) {
                result.duration = parseInt(durationMatch[1], 10);
            }

            const seedMatch = params.match(/Prefix of output trace file: \S+-(\d+)\.tr/);
            if (seedMatch) {
                result.seed = parseInt(seedMatch[1], 10);
            }
        }
        return result
    }

    private parseSimulationResults(input: string) {
        const regex = /Throughput \(Kbps\): (\d+\.\d+),(\d+\.\d+),/i;
        const totalRegex = /Total Throughput \(Kbps\): (\d+\.\d+)/i;
        const lostRegex = /Lost Packet Count: (\d+)/i;
        const throughputMatch = input.match(regex);
        const totalThroughputMatch = input.match(totalRegex);
        const lostPacketCountMatch = input.match(lostRegex);
      
        if (!throughputMatch || !totalThroughputMatch || !lostPacketCountMatch) {
          return null;
        }
      
        const throughput = throughputMatch.slice(1, 3).map((val) => parseFloat(val));
        const totalThroughput = parseFloat(totalThroughputMatch[1]);
        const lostPacketCount = parseInt(lostPacketCountMatch[1]);
      
        return { throughput, totalThroughput, lostPacketCount };
      }

    private parseSimulationResult(data: string): SimulationResult {
        const result: SimulationResult = {
            parameters: {
                algorithm: '',
                flows: 0,
                packet_error_rate: 0,
                bottleneck_bandwidth: '',
                bottleneck_delay: '',
                data_to_transmit_kb: 0,
                mtu: 0,
                duration: 0,
                seed: 0,
            },
            results: {
                throughput: 0,
                packet_loss: 0,
            },
            data: {
                throughput: [],
                cwnd_progression: [],
            },
        };
        result.parameters = this.parseSimulationParameters(data);
        result.data.cwnd_progression = this.parseCongestionWindowProgression(data);
        const parsedResults = this.parseSimulationResults(data);
        if (parsedResults) {
            result.data.throughput = parsedResults.throughput;
            result.results.throughput = parsedResults.totalThroughput;
            result.results.packet_loss = parsedResults.lostPacketCount;
        }
        console.log(JSON.stringify(result))
        return result;
    }

    private async runSimulation(params: SimulationParameters): Promise<SimulationResult> {
        const process = this.startNs3Process(params)
        if (params.algorithm === 'TcpMl') {
            this.startRlAgentProcess()
        }
        return new Promise((resolve, reject) => {
            let result: SimulationResult | undefined = undefined
            process.stdout?.on('data', (data) => {
                try {
                    result = this.parseSimulationResult(data.toString())
                } catch (e) {
                    console.log('error', e)
                }
            })
            process.on('close', (code) => {
                if (code !== 0 || !process.stdout || result === undefined) {
                    reject('Failed to run simulation')
                } else {
                    resolve(result)
                }
            })
        })
    }

    private startNs3Process(params: SimulationParameters): ChildProcessWithoutNullStreams {
        try {
            const proc = spawn(`./ns3 run scratch/congestion_simulations/main.cc -- --transport_prot=${params.algorithm} --run=${params.seed} --data=${params.data_to_transmit_kb} --mtu=${params.mtu} --num_flows=${params.flows} --duration=${params.duration} --bandwidth=${params.bottleneck_bandwidth} --delay=${params.bottleneck_delay} --prefix_name=out --tracing --flow_monitor`, {
                cwd: '/Users/swanav/Study/sem2/ee595-project/simulator/ns-allinone-3.38/ns-3.38',
                shell: true,
                env: {
                    ...process.env,
                    NS_GLOBAL_VALUE: 'SharedMemoryKey=4321;SharedMemoryPoolSize=4096;',
                }
            })

            proc.on('close', (code) => {
                console.log(`ns3 process exited with code ${code}`)
            })

            proc.on('error', (err) => {
                console.error(err)
            })

            proc.on('disconnect', () => {
                console.log('ns3 process disconnected')
            })

            proc.on('spawn', () => {
                console.log('ns3 process spawned')
            })

            proc.on('message', (message, sendHandle) => {
                console.log(`ns3 process message: ${message}`)
            })

            return proc
        } catch (ex) {
            console.log(ex)
            throw ex
        }
    }

    private startRlAgentProcess(): ChildProcessWithoutNullStreams {

        const proc = spawn('python3 rl_agent.py', {
            cwd: '/Users/swanav/Study/sem2/ee595-project/simulator/ns-allinone-3.38/ns-3.38/scratch/congestion_simulations',
            shell: true,
            env: {
                ...process.env,
                NS_GLOBAL_VALUE: 'SharedMemoryKey=4321;SharedMemoryPoolSize=4096;',
            }
        })

        proc.stdout.on('data', (data) => {
            console.log(data.toString())
        })

        proc.stderr.on('data', (data) => {
            console.error(data.toString())
        })

        proc.on('close', (code) => {
            console.log(`rl agent exited with code ${code}`)
        })
 
        return proc
    }
}
