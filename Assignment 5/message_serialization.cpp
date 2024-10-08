//implememtation of serialization functions
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message_serialization.h"
#include <iostream>

// Function to encode a Message object into a string format
void MessageSerialization::encode(const Message &msg, std::string &encoded_msg) {
    // Initialize the encoded message as an empty string
    encoded_msg = "";

    // Switch statement to handle different message types
    switch (msg.get_message_type()) {
        case MessageType::NONE:
            // Encode NONE message type
            encoded_msg += "NONE";
            break;
        case MessageType::LOGIN:
            // Encode LOGIN message type with username
            encoded_msg += "LOGIN";
            encoded_msg += " " + msg.get_username();
            break;
        case MessageType::CREATE:
            // Encode CREATE message type with table name
            encoded_msg += "CREATE";
            encoded_msg += " " + msg.get_table();
            break;
        case MessageType::PUSH:
            // Encode PUSH message type with value
            encoded_msg += "PUSH";
            encoded_msg += " " + msg.get_value();
            break;
        case MessageType::POP:
            // Encode POP message type
            encoded_msg += "POP";
            break;
        case MessageType::TOP:
            // Encode TOP message type
            encoded_msg += "TOP";
            break;
        case MessageType::SET:
            // Encode SET message type with table name and key
            encoded_msg += "SET";
            encoded_msg += " " + msg.get_table();
            encoded_msg += " " + msg.get_key();
            break;
        case MessageType::GET:
            // Encode GET message type with table name and key
            encoded_msg += "GET";
            encoded_msg += " " + msg.get_table();
            encoded_msg += " " + msg.get_key();
            break;
        case MessageType::ADD:
            // Encode ADD message type
            encoded_msg += "ADD";
            break;
        case MessageType::SUB:
            // Encode SUB message type
            encoded_msg += "SUB";
            break;
        case MessageType::MUL:
            // Encode MUL message type
            encoded_msg += "MUL";
            break;
        case MessageType::DIV:
            // Encode DIV message type
            encoded_msg += "DIV";
            break;
        case MessageType::BEGIN:
            // Encode BEGIN message type
            encoded_msg += "BEGIN";
            break;
        case MessageType::COMMIT:
            // Encode COMMIT message type
            encoded_msg += "COMMIT";
            break;
        case MessageType::BYE:
            // Encode BYE message type
            encoded_msg += "BYE";
            break;
        case MessageType::OK:
            // Encode OK message type
            encoded_msg += "OK";
            break;
        case MessageType::FAILED:
            // Encode FAILED message type with quoted text
            encoded_msg += "FAILED";
            encoded_msg += " \"" + msg.get_quoted_text() + "\"";
            break;
        case MessageType::ERROR:
            // Encode ERROR message type with quoted text
            encoded_msg += "ERROR";
            encoded_msg += " \"" + msg.get_quoted_text() + "\"";
            break;
        case MessageType::DATA:
            // Encode DATA message type with value
            encoded_msg += "DATA";
            encoded_msg += " " + msg.get_value();
            break;
        default:
            // Handle unknown message types (should not happen)
            break;
    }

    // Append newline character to indicate end of message
    encoded_msg += "\n";

    // Check if the encoded message exceeds the maximum allowed length
    if (encoded_msg.length() > Message::MAX_ENCODED_LEN) {
        throw InvalidMessage("Encoded message exceeds the maximum message length\n");
    }
}

// Function to decode an encoded string back into a Message object
void MessageSerialization::decode(const std::string &encoded_msg_, Message &msg) {
    // Check if the encoded message exceeds the maximum allowed length
    if (encoded_msg_.length() > Message::MAX_ENCODED_LEN) {
        throw InvalidMessage("Encoded message exceeds the maximum message length\n");
    }

    // Check if the encoded message ends with a newline character
    if (encoded_msg_[encoded_msg_.length() - 1] != '\n') {
        throw InvalidMessage("Encoded message isn’t terminated by a newline character\n");
    }

    // Create a stringstream from the encoded message for parsing
    std::stringstream ss(encoded_msg_);
    std::string type;  // Variable to store the message type
    ss >> type;  // Read the message type from the stream
    std::string data;  // Variable to store additional data

    // Create a new Message object to store the decoded data
    Message new_msg;

    // Decode based on the message type
    if (type == "NONE") {
        // Decode NONE message type
        new_msg.set_message_type(MessageType::NONE);
        ss >> data;  // Read additional data (if any)
        msg.push_arg(data);
    } else if (type == "LOGIN") {
        // Decode LOGIN message type
        new_msg.set_message_type(MessageType::LOGIN);
        ss >> data;  // Read username
        new_msg.push_arg(data);
    } else if (type == "CREATE") {
        // Decode CREATE message type
        new_msg.set_message_type(MessageType::CREATE);
        ss >> data;  // Read table name
        new_msg.push_arg(data);
    } else if (type == "PUSH") {
        // Decode PUSH message type
        new_msg.set_message_type(MessageType::PUSH);
	ss >> data;  // Read value                                                                                                                                                                  
        new_msg.push_arg(data);
    } else if (type == "POP") {
        // Decode POP message type
        new_msg.set_message_type(MessageType::POP);
    } else if (type == "TOP") {
        // Decode TOP message type
        new_msg.set_message_type(MessageType::TOP);
    } else if (type == "SET") {
        // Decode SET message type
        new_msg.set_message_type(MessageType::SET);
        ss >> data;  // Read table name
        new_msg.push_arg(data);
        ss >> data;  // Read key
        new_msg.push_arg(data);
    } else if (type == "GET") {
        // Decode GET message type
        new_msg.set_message_type(MessageType::GET);
        ss >> data;  // Read table name
        new_msg.push_arg(data);
        ss >> data;  // Read key
        new_msg.push_arg(data);
    } else if (type == "ADD") {
        // Decode ADD message type
        new_msg.set_message_type(MessageType::ADD);
    } else if (type == "SUB") {
        // Decode SUB message type
        new_msg.set_message_type(MessageType::SUB);
    } else if (type == "MUL") {
        // Decode MUL message type
        new_msg.set_message_type(MessageType::MUL);
    } else if (type == "DIV") {
        // Decode DIV message type
        new_msg.set_message_type(MessageType::DIV);
    } else if (type == "BEGIN") {
        // Decode BEGIN message type
        new_msg.set_message_type(MessageType::BEGIN);
    } else if (type == "COMMIT") {
        // Decode COMMIT message type
        new_msg.set_message_type(MessageType::COMMIT);
    } else if (type == "BYE") {
        // Decode BYE message type
        new_msg.set_message_type(MessageType::BYE);
    } else if (type == "OK") {
        // Decode OK message type
        new_msg.set_message_type(MessageType::OK);
    } else if (type == "FAILED") {
        // Decode FAILED message type
        new_msg.set_message_type(MessageType::FAILED);
        // Ignore characters until the first double quote
        while (ss.peek() != '"') {
            ss.ignore();
        }
        ss.ignore();  // Ignore the double quote
        // Read quoted text until the next double quote
        std::getline(ss, data, '\"');
        new_msg.push_arg(data);
    } else if (type == "ERROR") {
        // Decode ERROR message type
        new_msg.set_message_type(MessageType::ERROR);
        // Ignore characters until the first double quote
        while (ss.peek() != '"') {
            ss.ignore();
        }
        ss.ignore();  // Ignore the double quote
        // Read quoted text until the next double quote
        std::getline(ss, data, '\"');
        new_msg.push_arg(data);
    } else if (type == "DATA") {
        // Decode DATA message type
        new_msg.set_message_type(MessageType::DATA);
        ss >> data;  // Read value
        new_msg.push_arg(data);
    } else {
      throw InvalidMessage("Message object isn't valid");
    }

    while (ss >> data) {
      new_msg.push_arg(data);
    }

    // Validate the decoded message
    if (!new_msg.is_valid()) {
        throw InvalidMessage("Message object isn't valid");
    }

    // Assign the newly decoded message to the output parameter
    msg = new_msg;
}
