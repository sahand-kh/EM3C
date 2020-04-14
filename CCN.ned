simple nodes
{
    parameters:
        @display("i=misc/node2");
    gates:
        inout gate[];
}

simple clusterHeads
{
    parameters:
        @display("i=abstract/accesspoint");
    gates:
        inout gate[];
}

simple SDNController
{
    parameters:
        @display("i=device/mainframe");
    gates:
        inout gate[];
}

simple gateways
{
    parameters:
        @display("i=device/antennatower");
    gates:
        inout gate[];
}

simple cloudServer
{
    parameters:
        @display("i=misc/cloud");
    gates:
        inout gate[];
}

simple users
{
    parameters:
        @display("i=abstract/person");
        double warmpUpTime = default(500);
    gates:
        inout gate[];
}

simple nodesType1 extends nodes
{
    parameters:
        @display("i=,red2");
        int type = default(1);
//        int storage = 17;
        int storage = 10; //60%
//        int storage = 14; //80%
//        int storage = 20; //120%
//        int storage = 24; //140%
        int battery = 4800;
}

simple nodesType2 extends nodes
{
    parameters:
        @display("i=,green3");
        int type = default(2);
//        int storage = 22;
        int storage = 13; //60%
//        int storage = 18; //80%
//        int storage = 26; //120%
//        int storage = 31; //140%
        int battery = 24000;
}

simple nodesType3 extends nodes
{
    parameters:
        @display("i=,blue3");
        int type = default(3);
//        int storage = 27;
        int storage = 16; //60%
//        int storage = 22; //80%
//        int storage = 32; //120%
//        int storage = 38; //140%
        int battery = 120000;

}

simple cluster1nodesType1 extends nodesType1
{
    parameters:
        int clusterNumber = default(1);
}

simple cluster1nodesType2 extends nodesType2
{
    parameters:
        int clusterNumber = default(1);
}

simple cluster1nodesType3 extends nodesType3
{
    parameters:
        int clusterNumber = default(1);
}

simple cluster2nodesType1 extends nodesType1
{
    parameters:
        int clusterNumber = default(2);
}

simple cluster2nodesType2 extends nodesType2
{
    parameters:
        int clusterNumber = default(2);
}

simple cluster2nodesType3 extends nodesType3
{
    parameters:
        int clusterNumber = default(2);
}

simple cluster3nodesType1 extends nodesType1
{
    parameters:
        int clusterNumber = default(3);
}

simple cluster3nodesType2 extends nodesType2
{
    parameters:
        int clusterNumber = default(3);
}

simple cluster3nodesType3 extends nodesType3
{
    parameters:
        int clusterNumber = default(3);
}

simple cluster4nodesType1 extends nodesType1
{
    parameters:
        int clusterNumber = default(4);
}

simple cluster4nodesType2 extends nodesType2
{
    parameters:
        int clusterNumber = default(4);
}

simple cluster4nodesType3 extends nodesType3
{
    parameters:
        int clusterNumber = default(4);
}

simple cluster5nodesType1 extends nodesType1
{
    parameters:
        int clusterNumber = default(5);
}

simple cluster5nodesType2 extends nodesType2
{
    parameters:
        int clusterNumber = default(5);
}

simple cluster5nodesType3 extends nodesType3
{
    parameters:
        int clusterNumber = default(5);
}

simple cluster6nodesType1 extends nodesType1
{
    parameters:
        int clusterNumber = default(6);
}

simple cluster6nodesType2 extends nodesType2
{
    parameters:
        int clusterNumber = default(6);
}

simple cluster6nodesType3 extends nodesType3
{
    parameters:
        int clusterNumber = default(6);
}

simple cluster7nodesType1 extends nodesType1
{
    parameters:
        int clusterNumber = default(7);
}

simple cluster7nodesType2 extends nodesType2
{
    parameters:
        int clusterNumber = default(7);
}

simple cluster7nodesType3 extends nodesType3
{
    parameters:
        int clusterNumber = default(7);
}

simple cluster8nodesType1 extends nodesType1
{
    parameters:
        int clusterNumber = default(8);
}

simple cluster8nodesType2 extends nodesType2
{
    parameters:
        int clusterNumber = default(8);
}

simple cluster8nodesType3 extends nodesType3
{
    parameters:
        int clusterNumber = default(8);
}


network IoTNetwork
{
    types:
        channel insideClusterChannel extends ned.DelayChannel
        {
            delay = 5ms;
        }
        channel interClusterChannel extends ned.DelayChannel
        {
            delay = 25ms;
        }
        channel userClusterChannel extends ned.DelayChannel
        {
            delay = 5ms;
        }
        channel SDNChannel extends ned.DelayChannel
        {
            delay = 5ms;
        }
        channel cloudChannel extends ned.DelayChannel
        {
            delay = 250ms;
        }

    submodules:
        sdn: SDNController {
            @display("p=416,145");
        }
        cloud: cloudServer {
            @display("p=405,42;is=vl");
        }
        ch1: clusterHeads {
            @display("p=195,631");
        }
        ch2: clusterHeads {
            @display("p=426,631");
        }
        ch3: clusterHeads {
            @display("p=644,631");
        }
        ch4: clusterHeads {
            @display("p=405,470");
        }
        ch5: clusterHeads {
            @display("p=671,395");
        }
        ch6: clusterHeads {
            @display("p=162,374");
        }
        ch7: clusterHeads {
            @display("p=671,227");
        }
        ch8: clusterHeads {
            @display("p=162,213");
        }
        user1: users {
            @display("p=755,272");
        }
        user2: users {
            @display("p=68,272");
        }
        gateway1: gateways {
            @display("p=671,92");
        }
        gateway2: gateways {
            @display("p=162,92");
        }
        cl1Type1[4]: cluster1nodesType1 {
            @display("p=209,692");
        }
        cl1Type2[3]: cluster1nodesType2 {
            @display("p=144,682");
        }
        cl1Type3[2]: cluster1nodesType3 {
            @display("p=94,643");
        }
        cl2Type1[3]: cluster2nodesType1 {
            @display("p=347,692");
        }
        cl2Type2[5]: cluster2nodesType2 {
            @display("p=499,702");
        }
        cl2Type3[2]: cluster2nodesType3 {
            @display("p=436,702");
        }
        cl3Type1[3]: cluster3nodesType1 {
            @display("p=702,673");
        }
        cl3Type2[2]: cluster3nodesType2 {
            @display("p=746,604");
        }
        cl3Type3[5]: cluster3nodesType3 {
            @display("p=632,692");
        }
        cl4Type1[7]: cluster4nodesType1 {
            @display("p=366,374");
        }
        cl4Type2[6]: cluster4nodesType2 {
            @display("p=327,467");
        }
        cl4Type3[9]: cluster4nodesType3 {
            @display("p=499,484");
        }
        cl5Type1[5]: cluster5nodesType1 {
            @display("p=761,467");
        }
        cl5Type2[3]: cluster5nodesType2 {
            @display("p=793,406");
        }
        cl5Type3[4]: cluster5nodesType3 {
            @display("p=785,344");
        }
        cl6Type1[2]: cluster6nodesType1 {
            @display("p=36,324");
        }
        cl6Type2[4]: cluster6nodesType2 {
            @display("p=69,383");
        }
        cl6Type3[4]: cluster6nodesType3 {
            @display("p=47,445");
        }
        cl7Type1[4]: cluster7nodesType1 {
            @display("p=785,131");
        }
        cl7Type2[5]: cluster7nodesType2 {
            @display("p=761,191");
        }
        cl7Type3[3]: cluster7nodesType3 {
            @display("p=702,145");
        }
        cl8Type1[6]: cluster8nodesType1 {
            @display("p=105,110");
        }
        cl8Type2[5]: cluster8nodesType2 {
            @display("p=94,177");
        }
        cl8Type3[4]: cluster8nodesType3 {
            @display("p=209,131");
        }
    connections:
        cloud.gate++ <--> cloudChannel <--> gateway1.gate++; //gate0 gate0
        cloud.gate++ <--> cloudChannel <--> gateway2.gate++; //gate1 gate0

        sdn.gate++ <--> SDNChannel <--> ch1.gate++; //gate0 gate0
        sdn.gate++ <--> SDNChannel <--> ch2.gate++; //gate1 gate0
        sdn.gate++ <--> SDNChannel <--> ch3.gate++; //gate2 gate0
        sdn.gate++ <--> SDNChannel <--> ch4.gate++; //gate3 gate0
        sdn.gate++ <--> SDNChannel <--> ch5.gate++; //gate4 gate0
        sdn.gate++ <--> SDNChannel <--> ch6.gate++; //gate5 gate0
        sdn.gate++ <--> SDNChannel <--> ch7.gate++; //gate6 gate0
        sdn.gate++ <--> SDNChannel <--> ch8.gate++; //gate7 gate0

        user1.gate++ <--> userClusterChannel <--> ch1.gate++;  //gate0 gate1
        user1.gate++ <--> userClusterChannel <--> ch2.gate++;  //gate1 gate1
        user1.gate++ <--> userClusterChannel <--> ch3.gate++;  //gate2 gate1
        user1.gate++ <--> userClusterChannel <--> ch4.gate++;  //gate3 gate1
        user1.gate++ <--> userClusterChannel <--> ch5.gate++;  //gate4 gate1
        user1.gate++ <--> userClusterChannel <--> ch6.gate++;  //gate5 gate1
        user1.gate++ <--> userClusterChannel <--> ch7.gate++;  //gate6 gate1
        user1.gate++ <--> userClusterChannel <--> ch8.gate++;  //gate7 gate1

        user2.gate++ <--> userClusterChannel <--> ch1.gate++;  //gate0 gate2
        user2.gate++ <--> userClusterChannel <--> ch2.gate++;  //gate1 gate2
        user2.gate++ <--> userClusterChannel <--> ch3.gate++;  //gate2 gate2
        user2.gate++ <--> userClusterChannel <--> ch4.gate++;  //gate3 gate2
        user2.gate++ <--> userClusterChannel <--> ch5.gate++;  //gate4 gate2
        user2.gate++ <--> userClusterChannel <--> ch6.gate++;  //gate5 gate2
        user2.gate++ <--> userClusterChannel <--> ch7.gate++;  //gate6 gate2
        user2.gate++ <--> userClusterChannel <--> ch8.gate++;  //gate7 gate2


        gateway1.gate++ <--> interClusterChannel <--> ch7.gate++; //gate1 gate3
        gateway2.gate++ <--> interClusterChannel <--> ch8.gate++; //gate1 gate3

        ch7.gate++ <--> interClusterChannel <--> ch5.gate++; //gate4 gate3
        ch8.gate++ <--> interClusterChannel <--> ch6.gate++; //gate4 gate3
        ch4.gate++ <--> interClusterChannel <--> ch1.gate++; //gate3 gate3
        ch4.gate++ <--> interClusterChannel <--> ch2.gate++; //gate4 gate3
        ch4.gate++ <--> interClusterChannel <--> ch3.gate++; //gate5 gate3
        ch4.gate++ <--> interClusterChannel <--> ch5.gate++; //gate6 gate4
        ch4.gate++ <--> interClusterChannel <--> ch6.gate++; //gate7 gate4
//        ch4.gate++ <--> interClusterChannel <--> ch7.gate++; //gate8 gate5
//        ch4.gate++ <--> interClusterChannel <--> ch8.gate++; //gate9 gate5

        ch1.gate++ <--> insideClusterChannel <--> cl1Type1[0].gate++; //gate4 gate0
        ch1.gate++ <--> insideClusterChannel <--> cl1Type1[1].gate++; //gate5 gate0
        ch1.gate++ <--> insideClusterChannel <--> cl1Type1[2].gate++; //gate6 gate0
        ch1.gate++ <--> insideClusterChannel <--> cl1Type1[3].gate++; //gate7 gate0
        ch1.gate++ <--> insideClusterChannel <--> cl1Type2[0].gate++; //gate8 gate0
        ch1.gate++ <--> insideClusterChannel <--> cl1Type2[1].gate++; //gate9 gate0
        ch1.gate++ <--> insideClusterChannel <--> cl1Type2[2].gate++; //gate10 gate0
        ch1.gate++ <--> insideClusterChannel <--> cl1Type3[0].gate++; //gate11 gate0
        ch1.gate++ <--> insideClusterChannel <--> cl1Type3[1].gate++; //gate12 gate0

        ch2.gate++ <--> insideClusterChannel <--> cl2Type1[0].gate++; //gate4 gate0
        ch2.gate++ <--> insideClusterChannel <--> cl2Type1[1].gate++; //gate5 gate0
        ch2.gate++ <--> insideClusterChannel <--> cl2Type1[2].gate++; //gate6 gate0
        ch2.gate++ <--> insideClusterChannel <--> cl2Type2[0].gate++; //gate7 gate0
        ch2.gate++ <--> insideClusterChannel <--> cl2Type2[1].gate++; //gate8 gate0
        ch2.gate++ <--> insideClusterChannel <--> cl2Type2[2].gate++; //gate9 gate0
        ch2.gate++ <--> insideClusterChannel <--> cl2Type2[3].gate++; //gate10 gate0
        ch2.gate++ <--> insideClusterChannel <--> cl2Type2[4].gate++; //gate11 gate0
        ch2.gate++ <--> insideClusterChannel <--> cl2Type3[0].gate++; //gate12 gate0
        ch2.gate++ <--> insideClusterChannel <--> cl2Type3[1].gate++; //gate13 gate0

        ch3.gate++ <--> insideClusterChannel <--> cl3Type1[0].gate++; //gate4 gate0
        ch3.gate++ <--> insideClusterChannel <--> cl3Type1[1].gate++; //gate5 gate0
        ch3.gate++ <--> insideClusterChannel <--> cl3Type1[2].gate++; //gate6 gate0
        ch3.gate++ <--> insideClusterChannel <--> cl3Type2[0].gate++; //gate7 gate0
        ch3.gate++ <--> insideClusterChannel <--> cl3Type2[1].gate++; //gate8 gate0
        ch3.gate++ <--> insideClusterChannel <--> cl3Type3[0].gate++; //gate9 gate0
        ch3.gate++ <--> insideClusterChannel <--> cl3Type3[1].gate++; //gate10 gate0
        ch3.gate++ <--> insideClusterChannel <--> cl3Type3[2].gate++; //gate11 gate0
        ch3.gate++ <--> insideClusterChannel <--> cl3Type3[3].gate++; //gate12 gate0
        ch3.gate++ <--> insideClusterChannel <--> cl3Type3[4].gate++; //gate13 gate0

        ch4.gate++ <--> insideClusterChannel <--> cl4Type1[0].gate++; //gate10 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type1[1].gate++; //gate11 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type1[2].gate++; //gate12 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type1[3].gate++; //gate13 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type1[4].gate++; //gate14 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type1[5].gate++; //gate15 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type1[6].gate++; //gate16 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type2[0].gate++; //gate17 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type2[1].gate++; //gate18 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type2[2].gate++; //gate19 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type2[3].gate++; //gate20 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type2[4].gate++; //gate21 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type2[5].gate++; //gate22 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type3[0].gate++; //gate23 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type3[1].gate++; //gate24 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type3[2].gate++; //gate25 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type3[3].gate++; //gate26 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type3[4].gate++; //gate27 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type3[5].gate++; //gate28 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type3[6].gate++; //gate29 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type3[7].gate++; //gate30 gate0
        ch4.gate++ <--> insideClusterChannel <--> cl4Type3[8].gate++; //gate31 gate0

        ch5.gate++ <--> insideClusterChannel <--> cl5Type1[0].gate++; //gate5 gate0
        ch5.gate++ <--> insideClusterChannel <--> cl5Type1[1].gate++; //gate6 gate0
        ch5.gate++ <--> insideClusterChannel <--> cl5Type1[2].gate++; //gate7 gate0
        ch5.gate++ <--> insideClusterChannel <--> cl5Type1[3].gate++; //gate8 gate0
        ch5.gate++ <--> insideClusterChannel <--> cl5Type1[4].gate++; //gate9 gate0
        ch5.gate++ <--> insideClusterChannel <--> cl5Type2[0].gate++; //gate10 gate0
        ch5.gate++ <--> insideClusterChannel <--> cl5Type2[1].gate++; //gate11 gate0
        ch5.gate++ <--> insideClusterChannel <--> cl5Type2[2].gate++; //gate12 gate0
        ch5.gate++ <--> insideClusterChannel <--> cl5Type3[0].gate++; //gate13 gate0
        ch5.gate++ <--> insideClusterChannel <--> cl5Type3[1].gate++; //gate14 gate0
        ch5.gate++ <--> insideClusterChannel <--> cl5Type3[2].gate++; //gate15 gate0
        ch5.gate++ <--> insideClusterChannel <--> cl5Type3[3].gate++; //gate16 gate0

        ch6.gate++ <--> insideClusterChannel <--> cl6Type1[0].gate++; //gate5 gate0
        ch6.gate++ <--> insideClusterChannel <--> cl6Type1[1].gate++; //gate6 gate0
        ch6.gate++ <--> insideClusterChannel <--> cl6Type2[0].gate++; //gate7 gate0
        ch6.gate++ <--> insideClusterChannel <--> cl6Type2[1].gate++; //gate8 gate0
        ch6.gate++ <--> insideClusterChannel <--> cl6Type2[2].gate++; //gate9 gate0
        ch6.gate++ <--> insideClusterChannel <--> cl6Type2[3].gate++; //gate10 gate0
        ch6.gate++ <--> insideClusterChannel <--> cl6Type3[0].gate++; //gate11 gate0
        ch6.gate++ <--> insideClusterChannel <--> cl6Type3[1].gate++; //gate12 gate0
        ch6.gate++ <--> insideClusterChannel <--> cl6Type3[2].gate++; //gate13 gate0
        ch6.gate++ <--> insideClusterChannel <--> cl6Type3[3].gate++; //gate14 gate0

        ch7.gate++ <--> insideClusterChannel <--> cl7Type1[0].gate++; //gate6 gate0
        ch7.gate++ <--> insideClusterChannel <--> cl7Type1[1].gate++; //gate7 gate0
        ch7.gate++ <--> insideClusterChannel <--> cl7Type1[2].gate++; //gate8 gate0
        ch7.gate++ <--> insideClusterChannel <--> cl7Type1[3].gate++; //gate9 gate0
        ch7.gate++ <--> insideClusterChannel <--> cl7Type2[0].gate++; //gate10 gate0
        ch7.gate++ <--> insideClusterChannel <--> cl7Type2[1].gate++; //gate11 gate0
        ch7.gate++ <--> insideClusterChannel <--> cl7Type2[2].gate++; //gate12 gate0
        ch7.gate++ <--> insideClusterChannel <--> cl7Type2[3].gate++; //gate13 gate0
        ch7.gate++ <--> insideClusterChannel <--> cl7Type2[4].gate++; //gate14 gate0
        ch7.gate++ <--> insideClusterChannel <--> cl7Type3[0].gate++; //gate15 gate0
        ch7.gate++ <--> insideClusterChannel <--> cl7Type3[1].gate++; //gate16 gate0
        ch7.gate++ <--> insideClusterChannel <--> cl7Type3[2].gate++; //gate17 gate0

        ch8.gate++ <--> insideClusterChannel <--> cl8Type1[0].gate++; //gate6 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type1[1].gate++; //gate7 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type1[2].gate++; //gate8 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type1[3].gate++; //gate9 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type1[4].gate++; //gate10 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type1[5].gate++; //gate11 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type2[0].gate++; //gate12 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type2[1].gate++; //gate13 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type2[2].gate++; //gate14 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type2[3].gate++; //gate15 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type2[4].gate++; //gate16 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type3[0].gate++; //gate17 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type3[1].gate++; //gate18 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type3[2].gate++; //gate19 gate0
        ch8.gate++ <--> insideClusterChannel <--> cl8Type3[3].gate++; //gate19 gate0
}