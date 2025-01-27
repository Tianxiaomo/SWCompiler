#include "dotGen.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>

#include "graphIR/IRGraph.h"
#include "graphIR/OpNode.h"
#include "graphIR/TensorNode.h"

// using namespace std;

namespace swc {

    std::string dotGenIRNode(IRNode *irnode, std::string tensorInfo,
                         std::string opInfo) {

    std::string thisNode;
    std::vector<std::string> parentNodes;
    std::vector<std::string> childNodes;

    std::string str_tmp;
    std::string str_total;

    std::vector<std::string> NodeInfo;
    NodeInfo.push_back(tensorInfo);
    NodeInfo.push_back(opInfo);

    // init parentNodes
    for (int i = 0; i < irnode->parentNum(); ++i) {
        parentNodes.push_back(irnode->getParentNode(i)->name());
    }

    // init childNodes
    for (int i = 0; i < irnode->childNum(); ++i) {
        childNodes.push_back(irnode->getChildNode(i)->name());
    }

    // Generate dot codes.
    str_total = str_total + "    // Generate one Node!\n";

    // Generate the information of this Node
    // thisNode = irnode->name();
    //
    // NodeType nodeType = irnode->nodeType();
    //
    // if (nodeType == TENSOR_NODE)
    //     str_tmp = "    " + thisNode + NodeInfo[0];
    // else if (nodeType == OP_NODE)
    //     str_tmp = "    " + thisNode + NodeInfo[1];
    std::ostringstream os;
    os  << irnode;
    auto node_id_str = "node_" + os.str();

    NodeType nodeType = irnode->nodeType();

    if (nodeType == TENSOR_NODE)
        str_tmp = "\t" + node_id_str + NodeInfo[0];
    else if (nodeType == OP_NODE)
        str_tmp = "\t" + node_id_str + NodeInfo[1];

    str_total = str_total + str_tmp;

    // Generate -> Children
    /*
    for (int i = 0; i < irnode->childNum(); ++i) {
        os.str("");
        os << irnode->getChildNode(i);
        auto child_id_str = "node_"+os.str();
        str_tmp = "\t" + node_id_str + " -> " + child_id_str + ";\n";
        str_total = str_total + str_tmp;
    }
    */

    str_total = str_total + "\n";

    return str_total;
}

std::string dotGenIRNode(IRNode *irnode) {
    return dotGenIRNode(irnode, " [shape = box];\n", ";\n");
}

std::string dotGenTensorNode(TensorNode *tnode) {

    std::string tensorInfo = " [shape = record, ";
    std::string tensorName = tnode->name();

    // get NDim through "getTensor()->getNDim()"
    int NDim = tnode->getTensor()->getNDim();

    // generate the tensorInfo
    tensorInfo = tensorInfo + "label = \"{Name: " + tensorName + " |";

    std::ostringstream os;
    os << "Tensor: " << std::hex << tnode->getTensor() << " |";
    // os << "isExternal: " << tnode->isExternal() << " |";


    auto dev = tnode->getLabel()->getDeviceLabel();
    os << "Dev: (" << dev.rank << ", " << static_cast<int>(dev.type) << ", " << dev.id << ")\\l";
    tensorInfo += os.str();

    tensorInfo = tensorInfo + "NDim: " + std::to_string(NDim) + "\\l";

    // for (int i = 0; i < NDim; ++i) {
    //     if (i < NDim-1) {
    //         tensorInfo = tensorInfo + "Dim[" + std::to_string(i) + "]:" +
    //         std::to_string(tnode->getTensor()->getDim(i)) + " |";
    //     } else {
    //         tensorInfo = tensorInfo + "Dim[" + std::to_string(i) + "]:" +
    //         std::to_string(tnode->getTensor()->getDim(i)) + " }\"];\n";
    //     }
    // }
    tensorInfo += "Dims: [";
    for (int i = 0; i < NDim; ++i) {
        tensorInfo += std::to_string((unsigned long)tnode->getTensor()->getDim(i));
        if (i < NDim - 1)
            tensorInfo += ", ";
    }
    tensorInfo += "]\\l";
    auto layout = tnode->getTensor()->getMemLayoutTag();
    tensorInfo += "Layout: " + layout + "\\l";
    tensorInfo += "}\", ";
    tensorInfo += "color=cyan4, penwidth = 2";
    tensorInfo += "];\n";

    return dotGenIRNode(tnode, tensorInfo, ";\n");
}

std::string dotGenOpNode(OpNode *opnode) {

    std::string opInfo = " [";
    std::string opNodeName = opnode->name();
    std::string opType = "BASIC_OP";
    std::string opName = opnode->getOp()->getOpName();

    if (opnode->getOp()->getOpType() == BASIC_OP)
        opType = "BASIC_OP";
    else if (opnode->getOp()->getOpType() == DL_OP)
        opType = "DL_OP";
    else if (opnode->getOp()->getOpType() == TENSOR_OP)
        opType = "TENSOR_OP";


    /*
    int nInput = opnode->getOp()->getnInput();
    int nOutput = opnode->getOp()->getnOutput();
    opInfo += "Operation: " + opName + "\\n";
    opInfo += "_nInput: " + std::to_string(nInput) + "\\n";
    opInfo += "_nOutput: " + std::to_string(nOutput) + "\", ";
    */

    // generate the opInfo
    opInfo += "label = \"Node's name: " + opNodeName + "\\n";

    SWLOG_DEBUG(1) << opNodeName << " getOpInfo: " << opnode->getOp()->getOpInfo() << "\n";

    opInfo += opnode->getOp()->getOpInfo() + "\", ";
    opInfo += "color=darkorange1, penwidth = 2";
    opInfo += "];\n";

    // return opInfo;
    return dotGenIRNode(opnode, " [shape = box];\n", opInfo);
}

void dotGen(IRGraph *graph, std::string dotFileName) {

    std::cout << "Generate the dotFile " << dotFileName << std::endl;

    std::string dot_Total;

    // Traversing the entire calculation graph through the topology.
    for (int i = 0; i < graph->topologyNum(); i++) {
        for (int j = 0; j < graph->getNumInTopoLevel(i); j++) {
            /*  graph.getNodeInTopo(i, j) will return the current node.
                The node is an IRNode instead of a specific TensorNode or
               OpNode. */
            if (graph->getNodeInTopo(i, j)->nodeType() == TENSOR_NODE)
                dot_Total =
                    dot_Total +
                    dotGenTensorNode((TensorNode *)graph->getNodeInTopo(i, j));
            else if (graph->getNodeInTopo(i, j)->nodeType() == OP_NODE)
                dot_Total = dot_Total +
                            dotGenOpNode((OpNode *)graph->getNodeInTopo(i, j));
        }
    }

    std::ostringstream os;
    for (int i = 0; i < graph->topologyNum(); i++) {
        for (int j = 0; j < graph->getNumInTopoLevel(i); j++) {
            auto from = graph->getNodeInTopo(i, j);

            for (int i = 0; i < from->childNum(); ++i) {
                auto to = from->getChildNode(i);
                os  << "node_" << from << " -> " 
                    << "node_" << to << "\n";

            }
        }
    }

    dot_Total += os.str();

    std::string dot_title = "digraph CG { \n";
    std::string dot_end = "\n}";

    // dotFile Genrate
    std::ofstream dotfile(dotFileName, std::fstream::out);

    dotfile << dot_title << std::endl;
    dotfile << dot_Total;
    dotfile << dot_end << std::endl;

    // make svg
    // std::string svgFileName = "IRGraph.svg";
    std::string svgFileName =
        dotFileName.substr(0, dotFileName.length() - 3) + "svg";
    std::string dotGenCMD = "dot -T svg " + dotFileName + " -o " + svgFileName;
    std::cout << dotGenCMD << "\n";

    char *cmd = (char *)dotGenCMD.data();

    assert(system(cmd) == 0);
}

// void dotGen(IRGraph *graph) { dotGen(graph, "IRGraph.dot"); }

} // namespace swc
