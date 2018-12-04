/*
 * IRNode.cpp
 * Copyright © 2018 Hongkun Yu <staryhk@gmail.com>
 *
 * @AUTHOR:      Hongkun Yu
 * @MAIL:        staryhk@gmail.com
 * @VERSION:     2018-11-23
 */


#include "IRNode.h"

namespace swc {

IRNode::IRNode()
{
  _fatherNode = NULL;
  _childNode = NULL;
}

IRNode::~IRNode() 
{
  printf("free:%s\n", _name.c_str());
}

} //namespace swc
