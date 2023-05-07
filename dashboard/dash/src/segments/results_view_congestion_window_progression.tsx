import { useEffect, useState } from "react";
import { Line } from "react-chartjs-2";
import { Container } from "semantic-ui-react";
import {reno_data, cubic_data, ml_tcp_data} from "../data";
// import data from "../results";

const reno_cwnd = reno_data.map((item) => item.cwnd);
// const labels = reno_data.map((item) => item.timestamp);

const cubic_cwnd = cubic_data.map((item) => item.cwnd);
const labels = cubic_data.map((item) => item.timestamp);

const tcp_ml_cwnd = ml_tcp_data.map((item) => item.cwnd);
// const labels = ml_tcp_data.map((item) => item.timestamp);

const getExperimentData = (experimentId: string, data: any) => {

    // return {}

    // if (data === undefined) {
    //     return {}
    // }

    // console.log(data)
    
    // console.log('Experiment Data', data)
    // const cwnd_progression = data.map((item: any) => item.result.data.cwnd_progression);
    // console.log(cwnd_progression)

    // // Collect all the unique timestamps
    // const timestamps = new Set();
    // cwnd_progression.forEach((algo_cwnd: any) => {
    //     console.log(algo_cwnd[0])
    //     algo_cwnd[0].map((item: any) => timestamps.add(item.time))
    // })
    // console.log(timestamps)
    // // Convert timestamps to an array and sort it
    // const timestamps_array = Array.from(timestamps);
    // timestamps_array.sort((a: any, b: any) => a - b);
    

    // // Find the cwnd values for each timestamp for each algorithm.
    // // If the algorithmm doesn't have a cwnd for a timestamp, use the previous cwnd.
    // const cwnds: number[][] = [];
    // cwnd_progression.forEach((algo_cwnd: any, index: number) => {
    //     cwnds.push([]);
    //     const algo_cwnds = algo_cwnd[0];
    //     let last_cwnd = 0;
    //     timestamps.forEach((timestamp) => {
    //         const cwnd = algo_cwnds.find((item: any) => item.time === timestamp);
    //         if (cwnd === undefined) {
    //             cwnds[index].push(last_cwnd);
    //         } else {
    //             last_cwnd = cwnd.cwnd;
    //             cwnds[index].push(cwnd.cwnd);
    //         }
    //     })
    // })

    // console.log(cwnds)

    // console.log('labels', labels)
    // const cwnd = reno_data.map((item) => item.cwnd);
    return {
        labels: labels,
        datasets: [
            {
                label: 'TCP New Reno',
                data: reno_cwnd,
                borderWidth: 2,
                borderColor: '#000000',
                backgroundColor: '#000000',
            },
            {
                label: 'TCP Cubic',
                data: cubic_cwnd,
                borderWidth: 2,
                borderColor: '#00FF00',
                backgroundColor: '#00FF00',
            },
            {
                label: 'TCP ML',
                data: tcp_ml_cwnd,
                borderWidth: 1,
                borderColor: '#FF0000',
                backgroundColor: '#FF0000',
            }
        ],
    }

}

export default function CongestionWindowProgressionView(props: {
    experimentId: string | undefined,
    experimentResults: any[]
}) {
    const { experimentId, experimentResults } = props;

    // Get experiment data on load
    const [experimentData, setExperimentData] = useState<any>();
    useEffect(() => {
        if (typeof experimentId === 'string') {
            const data = getExperimentData(experimentId, experimentResults)
            setExperimentData(data);
        } else {
            setExperimentData(undefined);
        }
    }, [experimentId, experimentResults]);

    return <Container>
        {
            experimentData === undefined ?
                <div>Loading...</div> :

                <Line
                    datasetIdKey='id'
                    data={experimentData}
                    options={{
                        plugins: {
                            title: {
                                text: 'Congestion Window Progression',
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
                                    text: 'Time (s)'
                                },
                                type: 'linear',
                            },
                            y: {
                                title: {
                                    display: true,
                                    text: 'Congestion Window (bytes)'
                                },
                                min: 0,
                            }
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