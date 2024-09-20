/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "chameleon_fw_common.hpp"

namespace ihd {

const char *chameleon_fw_comms::ACK_STR = "ACK";
const char *chameleon_fw_comms::NCK_STR = "NCK";

[[nodiscard]]
uint32_t chameleon_fw_comms::getSequence() const {
    return _sequence;
}

void chameleon_fw_comms::setSequence(uint32_t sequence) {
    _sequence = sequence;
}

std::string chameleon_fw_comms::getCommandString() {
    std::stringstream ss;
    printf("The sequence number:%d\n", _sequence);
    if (_sequence > 0) {
        ss << _sequence << " ";
    }
    printf("The sequence string:%s\n", ss.str().c_str());
    ss << _command->to_command_string();
    printf("The string:%s\n", ss.str().c_str());
    return ss.str();
}

/**
* @brief Tokenize the given vector according to the regex and remove the empty tokens.
*
* @param str
* @param re
* @return std::vector<std::string>
*/
std::vector <std::string> chameleon_fw_comms::tokenize(const std::string str, const std::regex re) {
    std::sregex_token_iterator it{str.begin(), str.end(), re, -1};
    std::vector <std::string> tokenized{it, {}};
    // Additional check to remove empty strings
    tokenized.erase(
            std::remove_if(tokenized.begin(), tokenized.end(), [](std::string const &s) {
                               return s.size() == 0;
                           }
            ), tokenized.end());
    return tokenized;
}


void chameleon_fw_comms::setResponse(const char *response) {
    int err = 0;

    const std::regex comma_regx(R"([,]+)");
    const std::regex space_regx(R"([\s]+)");
    const std::vector <std::string> tokenized = tokenize(std::string(response), comma_regx);
    printf("parsed response:%s", response);

    int i = 0;
    for (auto ptr = tokenized.begin(); ptr < tokenized.end(); ptr++) {
        printf(" tokenize[%d]:%s\n", i++, ptr->c_str());
    }

    if (tokenized.size() == 0) {
        err = -1;
    } else {
        if (tokenized[0] == ACK_STR) {
            _result = Result::ACK;
        } else if(tokenized[0] == NCK_STR) {
            _result = Result::NAK;
        } else {
            err = -1;
        }
    }
    if (!err && tokenized.size() > 1) {
        auto command_str = tokenized[1];
        const std::vector <std::string> cmd_tokenized = tokenize(std::string(command_str), space_regx);
        if (cmd_tokenized.size() == 0) {
            err = -1;
        }  else {
            const char *cmd = nullptr;
            if (_sequence > 0) {
                /* The response should include a leading sequence number */
                if (cmd_tokenized.size() != 2) {
                    err = -1;
                } else {
                    cmd = cmd_tokenized[1].c_str(); /* <seq> <cmd> */
                    try {
                        auto response_sequence_number = static_cast<uint32_t>(std::stoul(cmd_tokenized[0]));
                        printf("response seq:%d command seq:%d\n", response_sequence_number, _sequence);
                        if (response_sequence_number != _sequence) {
                            err = -1;
                        }
                    } catch(...) {
                        err = -1;
                        std::exception_ptr p = std::current_exception();
                        std::cout << (p ? p.__cxa_exception_type()->name() : "null") << std::endl;
                    }
                }
            } else {
                printf("token size:%zu\n", cmd_tokenized.size());

                if (cmd_tokenized.size() != 1) {
                    err = -1;
                } else {
                    /* No sequence number, just a command */
                    cmd = cmd_tokenized[0].c_str();
                }
            }
            if (!err && cmd != nullptr) {
                printf("command:%s\n", cmd);
                if (strcmp(_command->getCommand(), cmd) != 0) {
                    err = -1;
                }
            }
        }
    }
    if (err) {
        _result = Result::ERROR;
    }
    printf("command result:%d\n", _result);
}

void chameleon_fw_comms::setResponseTimedout() {
    printf("Timeout waiting for response\n");
}

}