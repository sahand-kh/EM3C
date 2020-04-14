message packetInMsg
{
    string header;
    int buffer_id; //a unique value used to track the buffered packet
    int total_len; //The length of the captured packet is indicated by total_len
    string in_port; //The port the packet was recieved on is indicated by in_port
    string reason; //The reason field indicates why the packet has been captured and forwarded (NoMatch, Action)
}