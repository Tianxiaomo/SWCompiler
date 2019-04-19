/*
 * OpNode.h
 * Copyright (C) 2018 Hongkun Yu <staryhk@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef OPNODE_H_
#define OPNODE_H_

#include "IRNode.h"

#include "op/Op.h"
#include <sstream>

namespace swc {

class OpNode : public IRNode {
  public:
    OpNode() : op_(NULL){};
    explicit OpNode(std::string name) : IRNode(OP_NODE, name){};
    explicit OpNode(std::string name, Op *op)
        : IRNode(OP_NODE, name), op_(op){};
    ~OpNode(){};

    void destroy() {
        // printf("free OpNode:%s\n", name().c_str());

        getOp()->destroy();
        getLabel()->destroy();
        // this->~OpNode();
    };

    void setOp(Op *op) { op_ = op; }

    Op *getOp() { return op_; }

    const std::string getOpName() { return op_->getOpName(); }

    OpNode *clone() const;
    OpNode *deepClone() const;
    std::string toString() const;
    void setRunOnce() { run_once_ = true; }
    bool runable() {
        bool run = run_;
        if (run_once_)
            run_ = false;
        return run;
    }

  private:
    Op *op_;
    bool run_{true};
    bool run_once_{false};
};

} // namespace swc
#endif /* !OPNODE_H_ */
