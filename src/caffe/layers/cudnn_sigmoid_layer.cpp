#ifdef USE_CUDNN
#include <algorithm>
#include <vector>

#include "caffe/layer.hpp"
#include "caffe/vision_layers.hpp"

namespace caffe {

template <typename Dtype, typename Mtype>
void CuDNNSigmoidLayer<Dtype,Mtype>::LayerSetUp(const vector<Blob<Dtype,Mtype>*>& bottom,
      const vector<Blob<Dtype,Mtype>*>& top) {
  SigmoidLayer<Dtype,Mtype>::LayerSetUp(bottom, top);
  // initialize cuDNN
  cudnn::createTensor4dDesc<Dtype>(&bottom_desc_);
  cudnn::createTensor4dDesc<Dtype>(&top_desc_);
  cudnnCreateActivationDescriptor(&activ_desc_);
  cudnnSetActivationDescriptor(activ_desc_, CUDNN_ACTIVATION_SIGMOID, CUDNN_PROPAGATE_NAN, 0.0);
  handles_setup_ = true;
}

template <typename Dtype, typename Mtype>
void CuDNNSigmoidLayer<Dtype,Mtype>::Reshape(const vector<Blob<Dtype,Mtype>*>& bottom,
      const vector<Blob<Dtype,Mtype>*>& top) {
  SigmoidLayer<Dtype,Mtype>::Reshape(bottom, top);
  const int N = bottom[0]->num();
  const int K = bottom[0]->channels();
  const int H = bottom[0]->height();
  const int W = bottom[0]->width();
  cudnn::setTensor4dDesc<Dtype>(&bottom_desc_, N, K, H, W);
  cudnn::setTensor4dDesc<Dtype>(&top_desc_, N, K, H, W);
}

template <typename Dtype, typename Mtype>
CuDNNSigmoidLayer<Dtype,Mtype>::~CuDNNSigmoidLayer() {
  // Check that handles have been setup before destroying.
  if (!handles_setup_) { return; }

  cudnnDestroyActivationDescriptor(this->activ_desc_);
  cudnnDestroyTensorDescriptor(this->bottom_desc_);
  cudnnDestroyTensorDescriptor(this->top_desc_);
}

INSTANTIATE_CLASS(CuDNNSigmoidLayer);

}  // namespace caffe
#endif
