import { Container, Form, Header, Icon, Segment } from "semantic-ui-react";
import { ActivePage } from "../App";
import { cancelExperiment } from "../api";

function SimulatingSegment(props: {
    experimentId: string,
    setParamsReady: (paramsReady: boolean) => void,
    setActivePage: (activePage: ActivePage) => void
}) {

    const { setParamsReady, setActivePage } = props;

    const { experimentId } = props;

    return <Segment>
        <Container fluid textAlign='center'>
            <Icon size='massive' name='connectdevelop' loading />
            <Header as='h1'>Please wait...</Header>
            <Header as='h5'>We are running the simulations according to your specifications</Header>

            <Header as='h5'>This may take a while</Header>

            <Form.Button onClick={async () => {
                try {
                    await cancelExperiment(experimentId);
                    setParamsReady(false);
                    setActivePage('experiment_setup')
                } catch (e) {
                    console.error(e);
                }
            }}>Cancel the experiment</Form.Button>
        </Container>
    </Segment>
}

export default SimulatingSegment;