import { EventEmitter } from "stream";

export type ExperimentParameters = {
    'congestionControl': {
      'newReno': boolean,
      'cubic': boolean,
      'vegas': boolean,
      'tcpMl': boolean
    },
    'seed': number,
    'iterations': number,
    'numberOfFlows': number,
    'mtu': number,
    'dataRate': number,
    'delay': number,
    'queueSize': number
  }

export type ExperimentId = string

export async function createExperiment(params: ExperimentParameters) : Promise<ExperimentId> {
    const result = await fetch('http://localhost:5500/experiment', {
        body: JSON.stringify(params),
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
    });
    if (result.status !== 200) {
        throw new Error(`Failed to create experiment`)
    }
    const experimentId = await result.json()
    return experimentId
}

export async function cancelExperiment(experimentId: ExperimentId) : Promise<void> {
    // Call a DELETE request to the backend
    const response = await fetch(`http://localhost:5500/experiment/${experimentId}`, {
        method: 'DELETE',
    })
    if (response.status !== 200) {
        throw new Error(`Failed to cancel experiment ${experimentId}`)
    }
}

// Open a websocket connection to the backend and listen for results
export async function listenForResults(experimentId: ExperimentId, callback: CallableFunction) : Promise<void> {
    const ws = new WebSocket(`ws://localhost:5500/experiment-status`)
    ws.onmessage = (event) => {
        callback('result', event.data)
    }
    ws.onerror = (event) => {
        callback('error', event)
    }
    ws.onclose = (event) => {
        callback('close', event)
    }
    ws.onopen = (event) => {
        ws.send(JSON.stringify({experiment_id: experimentId}))
    }
}


// export async function getExperimentStatus(experimentId: ExperimentId) : Promise<string> {

// }
// 