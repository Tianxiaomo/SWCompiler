/*************************************************************************
    > File Name: testLeNetImport.cpp
    > Author: wayne
    > Mail:
    > Created Time: 四  3/28 16:16:24 2019
 ************************************************************************/

#include "SWC.h"
#include "importer/Caffe2Importer.h"
#include <iostream>

using namespace swc;
using namespace std;

int main() {
    IRGraph *graph = new IRGraph();
    std::vector<TensorNode *> udef;
    auto *data = new TensorNode("data", {8, 28, 28, 1});
    data->getTensor()->setTensorInit(TensorInitType::FILE,
                                     "mnist_images_8.bin");
    udef.push_back(data);
    Caffe2Importer importer(graph, "./lenet_mnist/predict_net.pb",
                            "./lenet_mnist/init_net.pb", udef);

    //---------------------------------------------------------------
    auto softmax_t = graph->getNodeByName("softmax");
    auto *argmax_o = new OpNode("argmax", new ArgMaxOp(3));
    argmax_o->exlinkUpperNode(softmax_t);

    auto *top3_t =
        new TensorNode("top3", new Tensor({8, 3}, DataType::Int32_t), argmax_o);

    auto *print_o = new OpNode("print", new DebugOp());
    print_o->exlinkUpperNode(top3_t);

    graph->pushOpNode(argmax_o, print_o);
    graph->pushTensorNode(top3_t);
    //---------------------------------------------------------------

    graph->updateTopology();

    // Optimizer is a must because Codegen need label
    pass::Optimizer *opt = new pass::Optimizer(graph);
    opt->runOptimizer();

    dotGen(graph);

    CodegenConfig config;
    codegen::Codegen *cg = new codegen::Codegen(graph, config);
    string code = cg->generate();
    cout << code;

    return 0;
}
