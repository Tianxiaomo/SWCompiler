/*************************************************************************
    > File Name: AutoDiff.h
    > Author: wayne
    > Mail:
    > Created Time: 三  4/10 17:25:58 2019
 ************************************************************************/

struct TrainingProfile;

namespace swc {
class IRGraph;

/** \brief A function to build Training network
 *
 *  TrainingNet and InferNet share some tensors
 *  since multiple TensorNodes may refer to the same Tensor
 *  \param graph Inference network
 *  \param profile Training profile, like learning rate, decay etc.
 *  \return IRGraph for training
 */
IRGraph *getTrainNet(IRGraph *graph, TrainingProfile &profile);

} // namespace swc
