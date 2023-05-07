import { Form, Header, Input, Segment } from "semantic-ui-react";
import { ActivePage} from "../App";
import { ExperimentParameters, createExperiment } from "../api";
import NetworkTopology from "./network_topology_graphic";

const boundValue = (value: number, min: number, max: number) => Math.min(Math.max(value, min), max);

function ExperimentSetupSegment(props: {
    updateExperimentId: (experimentId: string) => void,
    setParamsReady: (paramsReady: boolean) => void,
    setActivePage: (activePage: ActivePage) => void
    experimentParameters: ExperimentParameters,
    setExperimentParameters: (experimentParameters: ExperimentParameters) => void
}) {

    const { setParamsReady, setActivePage } = props;
    const { experimentParameters, setExperimentParameters } = props;

    const handleRunSimulation = async () => {
        try {
            const experimentId = await createExperiment(experimentParameters);
            props.updateExperimentId(experimentId)
            setParamsReady(true);
            setActivePage('simulating');
        } catch(e) {
            setParamsReady(false);
            console.error(e);
        }
    }

    return <Segment>
        <Header as='h1'>Experiment Setup</Header>
        <Header as='h5'>Choose the parameters for the experiment</Header>
        <NetworkTopology num_flows={experimentParameters.numberOfFlows} />;
        <Form>
            <Header as='h5' textAlign='left' >Choose the algorithms to run</Header>
            <Form.Group widths='equal'>
                <Form.Checkbox label='New Reno' onClick={() => {
                    const newExperimentParameters = { ...experimentParameters };
                    newExperimentParameters.congestionControl.newReno = !newExperimentParameters.congestionControl.newReno;
                    setExperimentParameters(newExperimentParameters)
                }} checked={experimentParameters.congestionControl.newReno} />
                <Form.Checkbox label='Cubic' onClick={() => {
                    const newExperimentParameters = { ...experimentParameters };
                    newExperimentParameters.congestionControl.cubic = !newExperimentParameters.congestionControl.cubic;
                    setExperimentParameters(newExperimentParameters)
                }} checked={experimentParameters.congestionControl.cubic} />
                <Form.Checkbox label='Vegas' onClick={() => {
                    const newExperimentParameters = { ...experimentParameters };
                    newExperimentParameters.congestionControl.vegas = !newExperimentParameters.congestionControl.vegas;
                    setExperimentParameters(newExperimentParameters)
                }} checked={experimentParameters.congestionControl.vegas} />
                <Form.Checkbox label='TCP-ML'onClick={() => {
                    const newExperimentParameters = { ...experimentParameters };
                    newExperimentParameters.congestionControl.tcpMl = !newExperimentParameters.congestionControl.tcpMl;
                    setExperimentParameters(newExperimentParameters)
                }} checked={experimentParameters.congestionControl.tcpMl} />
            </Form.Group>
            <Form.Group widths='equal'>
                <Form.Input fluid label='Number of flows' placeholder='Number of flows' type='number' min={0} max={5} value={experimentParameters.numberOfFlows} onChange={
                    (event, data) => {
                        const newExperimentParameters = { ...experimentParameters };
                        newExperimentParameters.numberOfFlows = boundValue(Number(data.value), 1, 5);
                        setExperimentParameters(newExperimentParameters)
                    }
                } />
            </Form.Group>
            <Form.Group widths='equal'>
                <Form.Input fluid label='MTU' value={experimentParameters.mtu}
                    input={<Input label={{ basic: true, content: 'bytes' }} labelPosition='right' min={1} max={1500} placeholder='Size of the segments' type='number' onChange={
                        (event, data) => {
                            const newExperimentParameters = { ...experimentParameters };
                            newExperimentParameters.mtu = boundValue(Number(data.value), 1, 1500);
                            setExperimentParameters(newExperimentParameters)
                        }
                    } />} />
                <Form.Input fluid label='Data Rate' value={experimentParameters.dataRate}
                    input={<Input label={{ basic: true, content: 'kbps' }} labelPosition='right' min={1} max={500} placeholder='Data Rate of the transmission' type='number' onChange={
                        (event, data) => {
                            const newExperimentParameters = { ...experimentParameters };
                            newExperimentParameters.dataRate = boundValue(Number(data.value), 1, 500);
                            setExperimentParameters(newExperimentParameters)
                        }
                    } />} />
            </Form.Group>
            <Form.Group widths='equal'>
                <Form.Input fluid label='Seed' value={experimentParameters.seed}
                    input={<Input min={1} max={100} placeholder='Size of the segments' type='number' onChange={
                        (event, data) => {
                            const newExperimentParameters = { ...experimentParameters };
                            newExperimentParameters.seed = boundValue(Number(data.value), 1, 1500);
                            setExperimentParameters(newExperimentParameters)
                        }
                    } />} />
                <Form.Input fluid label='Iterations for the TCP-ML' value={experimentParameters.iterations}
                    input={<Input min={1} max={500} placeholder='Number of iterations to provide the RL-agent to learn' type='number' onChange={
                        (event, data) => {
                            const newExperimentParameters = { ...experimentParameters };
                            newExperimentParameters.iterations = boundValue(Number(data.value), 1, 500);
                            setExperimentParameters(newExperimentParameters)
                        }
                    } />} />
            </Form.Group>
            <Form.Button onClick={() => { handleRunSimulation() }}>Run Simulation</Form.Button>
        </Form>
    </Segment>
}

export default ExperimentSetupSegment;