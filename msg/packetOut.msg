cplusplus {{
    #include <map>
    #include <string>
    typedef std::map<std::string,std::string> actionList;
}};

class noncobject actionList;

message packetOutMsg
{
    string header;
    int buffer_id; //a unique value used to track the buffered packet
    int actions_len; //Size of action array
    actionList action;  //list of actions to apply to the raw packet (Output (nextNodeName), Drop, Set TTL, Experimenter)
    string in_port; //If OFPP_TABLE is specified as the output port of an action, the in_port in the packet_out message is used in the flow table lookup.
}