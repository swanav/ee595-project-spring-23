import { Container, Icon, Segment, Step } from "semantic-ui-react";
import { ActivePage } from "../App";

function StageStepper(props: {
    activePage: ActivePage,
    setActivePage: (activePage: ActivePage) => void,
    paramsReady: boolean,
    resultsReady: boolean
}) {
    const { activePage, setActivePage, paramsReady, resultsReady } = props;
    return <Segment>
        <Container>
            <Step.Group>
                <Step icon='computer' title='Experiment Setup' description='Choose the test parameters' active={activePage === 'experiment_setup'} onClick={() => setActivePage('experiment_setup')} />
                <Step icon={<Icon loading={activePage === 'simulating'} name='connectdevelop' />} title='Simulating' description='Running the simulation in NS-3' active={activePage === 'simulating'} disabled={!paramsReady} onClick={() => setActivePage('simulating')} />
                <Step icon='chart line' title='Results' description='View the results' active={activePage === 'results'} disabled={!resultsReady} onClick={() => setActivePage('results')} />
            </Step.Group>
        </Container>
    </Segment>
}

export default StageStepper;