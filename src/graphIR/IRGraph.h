/*
 * IRGraph.h
 * Copyright (C) 2018 Hongkun Yu <staryhk@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef IRGRAPH_H_
#define IRGRAPH_H_

#include <iostream>
#include <vector>

#include "common.h"

namespace swc {

// Forward declarations
class TensorNode;
class OpNode;
class IRNode;

/**
 * @brief IR Node Graph class
 */
class IRGraph {
  public:
    IRGraph(){};
    ~IRGraph(){};

    TensorNode *getTensorNode(int i) const { return _tensors[i]; }
    OpNode *getOpNode(int i) const { return _ops[i]; }

    TensorNode *getInNode(int i) const { return _inNodes[i]; }
    TensorNode *getOutNode(int i) const { return _outNodes[i]; }

    int getNumInTopoLevel(int i) const { return _nodesByTopology[i].size(); }
    std::vector<IRNode *> getNodeInTopoLevel(int i) const {
        return _nodesByTopology[i];
    }
    IRNode *getNodeInTopo(int i, int j) const { return _nodesByTopology[i][j]; }
    IRNode *getNodeByName(std::string name) const;

    /// \brief extract subGraph by in/out \p TensorNode
    /// \return OpNode*(SubGraphOp*)->subG(type: IRGraph)
    OpNode *extractSubGraph(TensorNode *in, TensorNode *out);

    // parallel through split `in`
    // e.g. SLICE in on axis 0(horizonal), num=2
    bool buildSubGraphs(TensorNode *in, TensorNode *out,
                        ParallelStrategy strategy, int axis = 0, int num = 2);

    void initTensorNodes();

    // GraphStructure Construct Interface
    void pushTensorNode(){};
    template <typename T, typename... Types>
    void pushTensorNode(const T &firstArg, const Types &... args) {
        _tensors.push_back(firstArg);
        pushTensorNode(args...);
    }

    void delTensorNode(){};
    template <typename T, typename... Types>
    void delTensorNode(const T &firstArg, const Types &... args) {
        if (!delVecMember(_tensors, firstArg)) {
            std::cout << "Del Tensor Failed" << firstArg->name() << std::endl;
        }
        delTensorNode(args...);
    }
    void pushOpNode(){};
    template <typename T, typename... Types>
    void pushOpNode(const T &firstArg, const Types &... args) {
        _ops.push_back(firstArg);
        pushOpNode(args...);
    }

    void delOpNode(){};
    template <typename T, typename... Types>
    void delOpNode(const T &firstArg, const Types &... args) {
        if (!delVecMember(_ops, firstArg)) {
            std::cout << "Del Op Failed" << firstArg->name() << std::endl;
        }
        delOpNode(args...);
    }

    void pushInNode(){};
    template <typename T, typename... Types>
    void pushInNode(const T &firstArg, const Types &... args) {
        _inNodes.push_back(firstArg);
        pushInNode(args...);
    }

    void pushOutNode(){};
    template <typename T, typename... Types>
    void pushOutNode(const T &firstArg, const Types &... args) {
        _outNodes.push_back(firstArg);
        pushOutNode(args...);
    }

    // To mark out node to avoid to be eliminated
    // by EliminationPass
    void setOutMark();
    
    inline int tensorNodeNum() const { return _tensors.size(); }
    inline int opNodeNum() const { return _ops.size(); }
    inline int inNodeNum() const { return _inNodes.size(); }
    inline int outNodeNum() const { return _outNodes.size(); }
    inline int topologyNum() const { return _nodesByTopology.size(); }

    void findInOut();

    template <typename T> void updateTopology(T node);

    void updateTopology();
    void updateTopoNodeList();
    void copyTo(IRGraph* graph) const;
    
    IRGraph *clone() const;
    void setDeviceLabel(Device dev);
    Device getDeviceLabel() { return _dev; }

    void setTrainDataNodes(TensorNode *label, TensorNode *data) {
        _input_label_node = label;
        _input_data_node = data;
    }
    TensorNode *getTrainLabelNode() { return _input_label_node; }
    TensorNode *getTrainDataNode() { return _input_data_node; }

    void setConfig(Config config) { _config = config; }
    Config getConfig() { return _config; }

  private:
    std::vector<TensorNode *> _tensors;
    std::vector<OpNode *> _ops;

    std::vector<TensorNode *> _inNodes;
    std::vector<TensorNode *> _outNodes;

    std::vector<std::vector<IRNode *>> _nodesByTopology;

    TensorNode *_input_data_node{nullptr};
    TensorNode *_input_label_node{nullptr};

    // for backend compilation
    Config _config;

    Device _dev;
};
} // namespace swc

#endif /* !IRGRAPH_H_ */
