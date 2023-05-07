"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
require("colors");
const cors_1 = __importDefault(require("cors"));
const dotenv_1 = require("dotenv");
const express_1 = __importStar(require("express"));
const http_1 = require("http");
const mongodb_1 = require("mongodb");
const ws_1 = __importDefault(require("ws"));
const experiment_1 = require("./experiment");
const simulation_orchestrator_1 = __importDefault(require("./simulation-orchestrator"));
(0, dotenv_1.config)();
const app = (0, express_1.default)();
const server = (0, http_1.createServer)(app);
const wss = new ws_1.default.Server({ server, path: '/experiment-status' });
app.use((0, cors_1.default)());
app.use((0, express_1.json)());
const client = new mongodb_1.MongoClient('mongodb://localhost:27017');
const db = client.db('ee595_project');
// make the db object available to all routes
app.use((req, res, next) => {
    // @ts-ignore
    req.db = db;
    next();
});
const PORT = process.env.PORT || 5500;
const ENV = process.env.NODE_ENV || "development";
app.post(experiment_1.setupExperimentEndpoint, experiment_1.setupExperimentHandler);
app.delete(experiment_1.cancelExperimentEndpoint, experiment_1.cancelExperimentHandler);
app.get(experiment_1.getExperimentEndpoint, experiment_1.getExperimentHandler);
const parseExperimentId = (message) => {
    try {
        const json = JSON.parse(message.toString());
        const experiment_id = json.experiment_id;
        return experiment_id;
    }
    catch (ex) {
        throw JSON.stringify({
            error: "Expected experiment id"
        });
    }
};
const websocketConnectionHandler = (ws) => {
    console.log("Websocket connection established");
    ws.on('close', () => {
        console.log("Websocket connection closed");
    });
    let experimentId = null;
    let experiment;
    ws.on('message', (message) => __awaiter(void 0, void 0, void 0, function* () {
        // Parse the request body for the experiment id
        if (experimentId === null) {
            try {
                experimentId = parseExperimentId(message);
                experiment = new simulation_orchestrator_1.default(db, experimentId);
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
                yield experiment.startSimulation();
            }
            catch (ex) {
                console.log(ex);
                if (typeof ex === 'string') {
                    ws.send(ex);
                }
                ws.close();
            }
        }
    }));
    ws.on('close', () => {
        experiment === null || experiment === void 0 ? void 0 : experiment.stopSimulation();
        experimentId = null;
    });
};
wss.on('connection', websocketConnectionHandler);
server.listen(PORT, () => console.log(`Backend server: `.magenta +
    ` Running in ${ENV} mode on port ${PORT}`.cyan));
