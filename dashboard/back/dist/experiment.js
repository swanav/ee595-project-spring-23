"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.getExperimentHandler = exports.getExperimentEndpoint = exports.getExperiment = exports.cancelExperimentHandler = exports.cancelExperimentEndpoint = exports.setupExperimentHandler = exports.setupExperimentEndpoint = void 0;
const mongodb_1 = require("mongodb");
const validateExperimentParameters = (experiment_parameters) => {
    // Check if atleast one of the congestion control algorithm is selected
    if (!experiment_parameters.congestionControl.newReno && !experiment_parameters.congestionControl.cubic && !experiment_parameters.congestionControl.vegas && !experiment_parameters.congestionControl.tcpMl) {
        return false;
    }
    // Check if the number of flows is valid
    if (experiment_parameters.numberOfFlows < 1 || experiment_parameters.numberOfFlows > 10) {
        return false;
    }
    // Check if the MTU is valid
    if (experiment_parameters.mtu < 1 || experiment_parameters.mtu > 1500) {
        return false;
    }
    // Check if the data rate is valid
    if (experiment_parameters.dataRate < 0 || experiment_parameters.dataRate > 1000) {
        return false;
    }
    // Check if the delay is valid
    if (experiment_parameters.delay < 0 || experiment_parameters.delay > 1000) {
        return false;
    }
    // Check if the queue size is valid
    if (experiment_parameters.queueSize < 1 || experiment_parameters.queueSize > 1000) {
        return false;
    }
    return true;
};
exports.setupExperimentEndpoint = '/experiment';
const setupExperimentHandler = (_req, res) => __awaiter(void 0, void 0, void 0, function* () {
    // Parse the request body for the experiment parameters
    // Create a new experiment object
    const experiment_parameters = _req.body;
    console.log(experiment_parameters);
    // Check if the experiment parameters are valid
    if (!validateExperimentParameters(experiment_parameters)) {
        return res.status(400).send(JSON.stringify("Invalid experiment parameters"));
    }
    // @ts-ignore
    const db = _req.db;
    // Add the experiment object to the database
    try {
        experiment_parameters.experimentStatus = "pending";
        const result = yield db.collection('experiments').insertOne(experiment_parameters);
        console.log(result);
        return res.status(200).send(JSON.stringify(result.insertedId.toHexString()));
    }
    catch (err) {
        console.log(err);
        return res.status(500).send(JSON.stringify("Internal server error"));
    }
});
exports.setupExperimentHandler = setupExperimentHandler;
exports.cancelExperimentEndpoint = '/experiment/:experiment_id';
const cancelExperimentHandler = (_req, res) => __awaiter(void 0, void 0, void 0, function* () {
    // Parse the request url for the experiment id
    const experiment_id = _req.params.experiment_id;
    console.log(experiment_id);
    // Check if the experiment id is valid
    if (!experiment_id) {
        return res.status(400).send(JSON.stringify("Invalid experiment id"));
    }
    // Check if the experiment id exists in the database
    // @ts-ignore
    const db = _req.db;
    try {
        const result = yield db.collection('experiments').findOne({ _id: mongodb_1.ObjectId.createFromHexString(experiment_id) });
        if (!result) {
            return res.status(400).send(JSON.stringify("Invalid experiment id"));
        }
        // Update the experiment status to cancelled
        const update_result = yield db.collection('experiments').updateOne({ _id: mongodb_1.ObjectId.createFromHexString(experiment_id) }, { $set: { experimentStatus: "cancelled" } });
        console.log(update_result);
        return res.status(200).send(JSON.stringify("Experiment cancelled successfully"));
    }
    catch (err) {
        console.log(err);
        return res.status(500).send(JSON.stringify("Internal server error"));
    }
});
exports.cancelExperimentHandler = cancelExperimentHandler;
const getExperiment = (db, experiment_id) => __awaiter(void 0, void 0, void 0, function* () {
    // Check if the experiment id is valid
    if (!experiment_id) {
        throw new Error("Invalid experiment id");
    }
    // Check if the experiment id exists in the database
    try {
        const result = yield db.collection('experiments').findOne({ _id: mongodb_1.ObjectId.createFromHexString(experiment_id) });
        if (!result) {
            throw new Error("Invalid experiment id");
        }
        return result;
    }
    catch (err) {
        console.log(err);
        throw new Error("Internal server error");
    }
});
exports.getExperiment = getExperiment;
exports.getExperimentEndpoint = '/experiment/:experiment_id';
const getExperimentHandler = (_req, res) => __awaiter(void 0, void 0, void 0, function* () {
    // Parse the request body for the experiment id
    const experiment_id = _req.params.experiment_id;
    console.log(experiment_id);
    // @ts-ignore
    const db = _req.db;
    try {
        const experiment = yield (0, exports.getExperiment)(db, experiment_id);
        return res.status(200).send(JSON.stringify(experiment));
    }
    catch (err) {
        console.log(err);
        return res.status(500).send(JSON.stringify("Internal server error"));
    }
});
exports.getExperimentHandler = getExperimentHandler;
