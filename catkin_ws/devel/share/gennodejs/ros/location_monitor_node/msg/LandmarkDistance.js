// Auto-generated. Do not edit!

// (in-package location_monitor_node.msg)


"use strict";

let _serializer = require('../base_serialize.js');
let _deserializer = require('../base_deserialize.js');
let _finder = require('../find.js');

//-----------------------------------------------------------

class LandmarkDistance {
  constructor() {
    this.name = '';
    this.distance = 0.0;
  }

  static serialize(obj, bufferInfo) {
    // Serializes a message object of type LandmarkDistance
    // Serialize message field [name]
    bufferInfo = _serializer.string(obj.name, bufferInfo);
    // Serialize message field [distance]
    bufferInfo = _serializer.float64(obj.distance, bufferInfo);
    return bufferInfo;
  }

  static deserialize(buffer) {
    //deserializes a message object of type LandmarkDistance
    let tmp;
    let len;
    let data = new LandmarkDistance();
    // Deserialize message field [name]
    tmp = _deserializer.string(buffer);
    data.name = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [distance]
    tmp = _deserializer.float64(buffer);
    data.distance = tmp.data;
    buffer = tmp.buffer;
    return {
      data: data,
      buffer: buffer
    }
  }

  static datatype() {
    // Returns string type for a message object
    return 'location_monitor_node/LandmarkDistance';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'e2f8ddf8c9e39a28149179429f5ae342';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    string name #Name of the landmark
    float64 distance #Distance to landmark, in meters
    
    `;
  }

};

module.exports = LandmarkDistance;
