import { useCallback, useEffect, useState } from 'react';
import './App.css';
import { Menu } from 'semantic-ui-react';
import SimulatingSegment from './segments/simulating_segment';
import ResultsSegment from './segments/results_segment';
import ExperimentSetupSegment from './segments/experiment_setup_segment';
import StageStepper from './segments/stage_stepper';
import { ExperimentParameters, listenForResults } from './api';

export type ActivePage = 'experiment_setup' | 'simulating' | 'results';

function App() {
  const [activePage, setActivePage] = useState<ActivePage>('results');
  const [paramsReady, setParamsReady] = useState<boolean>(false);
  const [resultsReady, setResultsReady] = useState<boolean>(true);
  const [experimentId, updateExperimentId] = useState<string|undefined>('undefined');
  const [experimentParameters, setExperimentParameters] = useState<ExperimentParameters>({
    'congestionControl': { 'cubic': true, 'newReno': true, 'tcpMl': true, 'vegas': false },
    'dataRate': 100,
    'delay': 0.001,
    'mtu': 1500,
    'numberOfFlows': 2,
    'queueSize': 100,
    'seed': 42,
    'iterations': 50
  });

  const [results, setResults] = useState<any[]>([]);

  useEffect(() => {
    if (experimentId !== undefined) {
      listenForResults(experimentId, function (event: string, data: any) {
        if (event === 'result') {
          setResultsReady(true);
          setActivePage('results');
          setResults([...results, JSON.parse(data)]);
          console.log(data);
        }
      });
    }
  }, [experimentId]);

  return (
    <div className="App">
      <Menu>
        <Menu.Item header>TCP-ML</Menu.Item>
        <Menu.Menu position='right'>
          <Menu.Item header>EE595 - Software Design and Optimization</Menu.Item>
        </Menu.Menu>
      </Menu>
      <StageStepper activePage={activePage} paramsReady={paramsReady} resultsReady={resultsReady} setActivePage={setActivePage} />
      {activePage === 'experiment_setup' && <ExperimentSetupSegment updateExperimentId={updateExperimentId} experimentParameters={experimentParameters} setActivePage={setActivePage} setExperimentParameters={setExperimentParameters} setParamsReady={setParamsReady} />}
      {activePage === 'simulating' && experimentId !== undefined && <SimulatingSegment experimentId={experimentId} setActivePage={setActivePage} setParamsReady={setParamsReady} />}
      {activePage === 'results' && <ResultsSegment experimentId={experimentId} experimentParameters={experimentParameters} experimentResults={results} />}
    </div>
  );
}


export default App;
