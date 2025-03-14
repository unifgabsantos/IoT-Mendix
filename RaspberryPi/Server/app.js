const moment = require('moment-timezone');
const mqtt = require('mqtt');
require('dotenv').config();
const { MongoClient } = require('mongodb');

const mqttHost = 'mqtt://raspberrypi';
const mqttPort = 1883;
const topic = 'mendix/iot';

const mongoUri = process.env.MONGO_URI;
const dbName = process.env.DB_NAME;
const collectionName = process.env.COLLECTION_NAME;

let mongoClient;

async function connectMongoDB() {
    mongoClient = new MongoClient(mongoUri);
    await mongoClient.connect();
    console.log('Conectado ao MongoDB Atlas');
}

async function saveToMongoDB(value) {
    try {
        const db = mongoClient.db(dbName);
        const collection = db.collection(collectionName);

        const document = {
            timeField: new Date(),
            value: parseFloat(value)
        };
        
        await collection.insertOne(document);
        console.log(`Temperatura: ${value} ºC`);
    } catch (error) {
        console.error('Erro ao salvar no MongoDB:', error);
    }
}

connectMongoDB().catch(console.error);


const client = mqtt.connect(mqttHost, { port: mqttPort });

client.on('connect', () => {
    console.log('Conectado ao broker MQTT');
    client.subscribe(topic, (err) => {
        if (!err) {
            console.log(`Inscrito no tópico: ${topic}`);
        } else {
            console.error('Erro na inscrição ao tópico:', err);
        }
    });
});

client.on('message', (receivedTopic, message) => {
    if (receivedTopic === topic) {
        const data = message.toString();
        saveToMongoDB(data);
    }
});

client.on('error', (err) => {
    console.error('Erro no MQTT:', err);
});
