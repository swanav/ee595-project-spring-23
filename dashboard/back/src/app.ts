import "colors";
import cors from "cors";
import { config } from "dotenv";
import express, { Application, json } from "express";
import {createServer as createHttpServer} from 'http';
import { MongoClient } from 'mongodb';
import Websocket from 'ws';

import { cancelExperimentEndpoint, cancelExperimentHandler, getExperimentEndpoint, getExperimentHandler, setupExperimentEndpoint, setupExperimentHandler } from "./experiment";
import SimulationOrchestrator from "./simulation-orchestrator";

config();

const app: Application = express();
const server = createHttpServer(app);
const wss = new Websocket.Server({ server, path: '/experiment-status' });

app.use(cors());
app.use(json());

const client = new MongoClient('mongodb://localhost:27017');
const db = client.db('ee595_project')
// make the db object available to all routes
app.use((req, res, next) => {
    // @ts-ignore
    req.db = db;
    next();
});

const PORT: string | number = process.env.PORT || 5500;
const ENV: string = process.env.NODE_ENV || "development";

app.post(setupExperimentEndpoint, setupExperimentHandler);
app.delete(cancelExperimentEndpoint, cancelExperimentHandler);
app.get(getExperimentEndpoint, getExperimentHandler);

const parseExperimentId = (message: Websocket.RawData) => {
  try {
    const json = JSON.parse(message.toString());
    const experiment_id: string = json.experiment_id;
    return experiment_id
  } catch(ex) {
    throw JSON.stringify({
      error: "Expected experiment id"
    });
  }
}

const websocketConnectionHandler = (ws: Websocket) => {
    console.log("Websocket connection established");
    ws.on('close', () => {
        console.log("Websocket connection closed");
    });

    let experimentId: string | null = null;
    let experiment: SimulationOrchestrator;

    ws.on('message', async (message) => {
      // Parse the request body for the experiment id
      if (experimentId === null) {
        try {
          experimentId = parseExperimentId(message)
          experiment = new SimulationOrchestrator(db, experimentId);
          experiment.on('result', (result) => {
            ws.send(JSON.stringify({
              result
            }));
          });
          experiment.on('status', (status) => {
            ws.send(JSON.stringify({
              status
            }));
          });
          experiment.on('completed', () => {
            ws.send(JSON.stringify({
              status: 'completed'
            }));
            ws.close();
          });
          await experiment.startSimulation();
        } catch(ex: any) {
          console.log(ex);
          if (typeof ex === 'string') {
            ws.send(ex);
          }
          ws.close();
        }
      }
    });
    ws.on('close', () => {
      experiment?.stopSimulation();
      experimentId = null;
    });
}


wss.on('connection', websocketConnectionHandler);

server.listen(PORT, () =>
  console.log(
    `Backend server: `.magenta +
      ` Running in ${ENV} mode on port ${PORT}`.cyan
  )
);
