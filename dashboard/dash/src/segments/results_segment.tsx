import { Grid, Header, Menu, Segment } from "semantic-ui-react";
import { ExperimentParameters } from "../api";
import { registerables, Chart as ChartJS } from 'chart.js/auto'
import 'chartjs-adapter-moment';
import { useState } from "react";
import ThroughputOverEpisodesView from "./results_view_throughput_over_episodes";
import CongestionWindowProgressionView from "./results_view_congestion_window_progression";

ChartJS.register(...registerables);

type OutputSelections = 'Congestion Window Progression' | 'Throughput over episodes';

const CongestionControlNames = {
    'newReno': 'New Reno',
    'cubic': 'Cubic',
    'vegas': 'Vegas',
    'tcpMl': 'TCP-ML'
}


function ResultsSegment(props: {
    experimentId: string | undefined,
    experimentParameters: ExperimentParameters
    experimentResults: any[]
}) {
    const { experimentParameters } = props;
    const [selectedOutput, setSelectedOutput] = useState<OutputSelections>('Congestion Window Progression');
    const { experimentResults } = props;

    return <Segment>
        <Header as='h1'>Results</Header>
        <Menu pointing>
            <Menu.Item header active={selectedOutput === 'Congestion Window Progression'} onClick={() => setSelectedOutput('Congestion Window Progression')}>
                Congestion Window Progression
            </Menu.Item>
            {
                Object.keys(experimentParameters.congestionControl).filter((key) => experimentParameters.congestionControl[key as keyof typeof experimentParameters.congestionControl] === true) //.filter((key) => experimentParameters.congestionControl[key as keyof typeof experimentParameters.congestionControl] === true)
                    .map((key) => <Menu.Item key={key}>{CongestionControlNames[key as keyof typeof experimentParameters.congestionControl]}</Menu.Item>)
            }
            <Menu.Menu position='right'>
                <Menu.Item header active={selectedOutput === 'Throughput over episodes'} onClick={() => setSelectedOutput('Throughput over episodes')}>
                    Throughput over episodes
                </Menu.Item>
            </Menu.Menu>
        </Menu>
        <Grid columns={2} >
            <Grid.Row columns={2} >
                {selectedOutput === 'Congestion Window Progression' &&
                    <CongestionWindowProgressionView experimentId={props.experimentId} experimentResults={experimentResults} />}
                {selectedOutput === 'Throughput over episodes' &&
                    <ThroughputOverEpisodesView experimentId={props.experimentId} experimentResults={experimentResults} />}
            </Grid.Row>
        </Grid>
    </Segment>
}

export default ResultsSegment;
