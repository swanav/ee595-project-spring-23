import { useEffect, useState } from "react";
import { Line } from "react-chartjs-2";
import { Container } from "semantic-ui-react";
import {ml_tcp_training_data, tcp_cubic_training_data, tcp_linux_reno_training_data, tcp_new_reno_training_data, tcp_vegas_training_data} from "../data";

const getExperimentData = (experimentId: string) => {

    const runs = ml_tcp_training_data.throughput.length

    const labels = Array.from({ length: runs }, (value, index) => `${index + 1}`);
    return {
        labels: labels,
        datasets: [
            {
                yAxisID: 'yThroughput',
                label: 'TCP New Reno (Throughput)',
                data: Array.from({ length: runs }, (value, index) => `${tcp_new_reno_training_data.throughput}`),
                borderWidth: 2,
                borderColor: '#0B3C49',
                backgroundColor: '#0B3C49',
            },
            {
                yAxisID: 'yThroughput',
                label: 'TCP Cubic (Throughput)',
                data: Array.from({ length: runs }, (value, index) => `${tcp_cubic_training_data.throughput}`),
                borderWidth: 2,
                borderColor: '#FA003F',
                backgroundColor: '#FA003F',
            },
            {
                yAxisID: 'yThroughput',
                label: 'TCP Linux Reno (Throughput)',
                data: Array.from({ length: runs }, (value, index) => `${tcp_linux_reno_training_data.throughput}`),
                borderWidth: 2,
                borderColor: '#00916E',
                backgroundColor: '#00916E',
            },
            {
                yAxisID: 'yThroughput',
                label: 'TCP Vegas (Throughput)',
                data: Array.from({ length: runs }, (value, index) => `${tcp_vegas_training_data.throughput}`),
                borderWidth: 2,
                borderColor: '#EE6123',
                backgroundColor: '#EE6123',
            },
            {
                label: 'TCP ML (Throughput)',
                yAxisID: 'yThroughput',
                data: ml_tcp_training_data.throughput,
                borderWidth: 1,
                borderColor: '#171738',
                backgroundColor: '#171738',
            },
            {
                yAxisID: 'yLoss',
                label: 'TCP New Reno (Drop)',
                data: Array.from({ length: runs }, (value, index) => `${tcp_new_reno_training_data.packets_lost}`),
                borderWidth: 2,
                borderColor: '#0B3C49',
                backgroundColor: '#0B3C49',
            },
            {
                yAxisID: 'yLoss',
                label: 'TCP Cubic (Drop)',
                data: Array.from({ length: runs }, (value, index) => `${tcp_cubic_training_data.packets_lost}`),
                borderWidth: 2,
                borderColor: '#FA003F',
                backgroundColor: '#FA003F',
            },
            {
                yAxisID: 'yLoss',
                label: 'TCP Linux Reno (Drop)',
                data: Array.from({ length: runs }, (value, index) => `${tcp_linux_reno_training_data.packets_lost}`),
                borderWidth: 2,
                borderColor: '#00916E',
                backgroundColor: '#00916E',
            },
            {
                yAxisID: 'yLoss',
                label: 'TCP Vegas (Drop)',
                data: Array.from({ length: runs }, (value, index) => `${tcp_vegas_training_data.packets_lost}`),
                borderWidth: 2,
                borderColor: '#EE6123',
                backgroundColor: '#EE6123',
            },
            {
                label: 'TCP ML (Drop)',
                yAxisID: 'yLoss',
                data: ml_tcp_training_data.packets_lost,
                borderWidth: 1,
                borderColor: '#DB5461',
                backgroundColor: '#DB5461',
            },
        ],
    }

}

export default function ThroughputOverEpisodesView(props: {
    experimentId: string | undefined,
    experimentResults: any[]
}) {

    const { experimentId } = props;

    // Get experiment data on load
    const [experimentData, setExperimentData] = useState<any>();
    useEffect(() => {
        if (typeof experimentId === 'string') {
            const data = getExperimentData(experimentId)
            setExperimentData(data);
        } else {
            setExperimentData(undefined);
        }
    }, [experimentId]);

    return <Container> {
        experimentData === undefined ? <div>Loading...</div> :
            <Line
                datasetIdKey='label_id_key_throughput_over_episodes'
                data={experimentData as any}
                options={{
                    plugins: {
                        title: {
                            text: 'Throughput over episodes',
                            display: true
                        },
                        legend: {
                            display: true,
                        }
                    },
                    scales: {
                        x: {
                            title: {
                                display: true,
                                text: 'Run'
                            },
                        },
                        yThroughput: {
                            title: {
                                display: true,
                                text: 'Throughput (Kbps)'
                            },
                            min: 0,
                            // max: 10000,
                        },
                        yLoss: {
                            title: {
                                display: true,
                                text: 'Loss'
                            },
                            min: 0,
                            max: 500,
                        },

                    },
                    elements: {
                        point: {
                            radius: 0
                        }
                    }
                } as any}
            />
    }
    </Container>
}