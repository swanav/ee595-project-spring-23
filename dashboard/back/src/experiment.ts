import express, { Application, json, Request, Response } from "express";
import { MongoClient, ObjectId } from 'mongodb';

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
    'queueSize': number,
    'experimentId': string,
    'experimentStatus': string,
    'experimentStartTime': string,
    'experimentEndTime': string,
}

const validateExperimentParameters = (experiment_parameters: ExperimentParameters) => {
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
}

export const setupExperimentEndpoint = '/experiment';
export const setupExperimentHandler = async (_req: Request, res: Response) => {
    // Parse the request body for the experiment parameters
    
    // Create a new experiment object
    const experiment_parameters: ExperimentParameters = _req.body;
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
        const result = await db.collection('experiments').insertOne(experiment_parameters)
        console.log(result);
        return res.status(200).send(JSON.stringify(result.insertedId.toHexString()));
    } catch (err) {
        console.log(err);
        return res.status(500).send(JSON.stringify("Internal server error"));
    }
}

export const cancelExperimentEndpoint = '/experiment/:experiment_id';
export const cancelExperimentHandler = async (_req: Request, res: Response) => {
    // Parse the request url for the experiment id
    const experiment_id: string = _req.params.experiment_id;
    console.log(experiment_id)

    // Check if the experiment id is valid
    if (!experiment_id) {
        return res.status(400).send(JSON.stringify("Invalid experiment id"));
    }

    // Check if the experiment id exists in the database
    // @ts-ignore
    const db = _req.db;
    try {
        const result = await db.collection('experiments').findOne({ _id: ObjectId.createFromHexString(experiment_id) });
        if (!result) {
            return res.status(400).send(JSON.stringify("Invalid experiment id"));
        }
        // Update the experiment status to cancelled
        const update_result = await db.collection('experiments').updateOne({ _id: ObjectId.createFromHexString(experiment_id) }, { $set: { experimentStatus: "cancelled" } });
        console.log(update_result);
        return res.status(200).send(JSON.stringify("Experiment cancelled successfully"));
    } catch (err) {
        console.log(err);
        return res.status(500).send(JSON.stringify("Internal server error"));
    }
}

export const getExperiment = async (db: any, experiment_id: string) => {
    // Check if the experiment id is valid
    if (!experiment_id) {
        throw new Error("Invalid experiment id");
    }
    
    // Check if the experiment id exists in the database
    try {
        const result = await db.collection('experiments').findOne({ _id: ObjectId.createFromHexString(experiment_id) });
        if (!result) {
            throw new Error("Invalid experiment id");
        }
        return result;
    } catch (err) {
        console.log(err);
        throw new Error("Internal server error");
    }

}

export const getExperimentEndpoint = '/experiment/:experiment_id';
export const getExperimentHandler = async (_req: Request, res: Response) => {
    // Parse the request body for the experiment id
    const experiment_id: string = _req.params.experiment_id;
    console.log(experiment_id)

    // @ts-ignore
    const db = _req.db;
    try {
        const experiment = await getExperiment(db, experiment_id);
        return res.status(200).send(JSON.stringify(experiment));
    } catch (err) {
        console.log(err);
        return res.status(500).send(JSON.stringify("Internal server error"));
    }
}
