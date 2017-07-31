#ifndef LIGHTGBM_TREELEARNER_SPLIT_INFO_HPP_
#define LIGHTGBM_TREELEARNER_SPLIT_INFO_HPP_

#include <LightGBM/meta.h>

#include <cmath>
#include <cstdint>
#include <cstring>

#include <functional>

namespace LightGBM {

/*!
* \brief Used to store some information for gain split point
*/
struct SplitInfo {
public:
  /*! \brief Feature index */
  int feature;
  /*! \brief Split threshold */
  uint32_t threshold;
  /*! \brief Left number of data after split */
  data_size_t left_count;
  /*! \brief Right number of data after split */
  data_size_t right_count;
  int num_cat_threshold;
  /*! \brief Left output after split */
  double left_output;
  /*! \brief Right output after split */
  double right_output;
  /*! \brief Split gain */
  double gain;
  /*! \brief Left sum gradient after split */
  double left_sum_gradient;
  /*! \brief Left sum hessian after split */
  double left_sum_hessian;
  /*! \brief Right sum gradient after split */
  double right_sum_gradient;
  /*! \brief Right sum hessian after split */
  double right_sum_hessian;
  std::vector<uint32_t> cat_threshold;
  /*! \brief True if default split is left */
  bool default_left;

  SplitInfo() {
    // initialize with -1 and -inf gain
    feature = -1;
    gain = kMinScore;
  }

  inline static int Size(int max_left_cat) {
    return 2 * sizeof(int) + sizeof(uint32_t) + sizeof(bool) + sizeof(double) * 7 + sizeof(data_size_t) * 2 + max_left_cat * sizeof(uint32_t);
  }

  inline void CopyTo(char* buffer) const {
    std::memcpy(buffer, &feature, sizeof(feature));
    buffer += sizeof(feature);
    std::memcpy(buffer, &left_count, sizeof(left_count));
    buffer += sizeof(left_count);
    std::memcpy(buffer, &right_count, sizeof(right_count));
    buffer += sizeof(right_count);
    std::memcpy(buffer, &gain, sizeof(gain));
    buffer += sizeof(gain);
    std::memcpy(buffer, &threshold, sizeof(threshold));
    buffer += sizeof(threshold);
    std::memcpy(buffer, &left_output, sizeof(left_output));
    buffer += sizeof(left_output);
    std::memcpy(buffer, &right_output, sizeof(right_output));
    buffer += sizeof(right_output);
    std::memcpy(buffer, &left_sum_gradient, sizeof(left_sum_gradient));
    buffer += sizeof(left_sum_gradient);
    std::memcpy(buffer, &left_sum_hessian, sizeof(left_sum_hessian));
    buffer += sizeof(left_sum_hessian);
    std::memcpy(buffer, &right_sum_gradient, sizeof(right_sum_gradient));
    buffer += sizeof(right_sum_gradient);
    std::memcpy(buffer, &right_sum_hessian, sizeof(right_sum_hessian));
    buffer += sizeof(right_sum_hessian);
    std::memcpy(buffer, &default_left, sizeof(default_left));
    buffer += sizeof(default_left);
    for (int i = 0; i < num_cat_threshold; ++i) {
      std::memcpy(buffer, &cat_threshold[i], sizeof(uint32_t));
      buffer += sizeof(uint32_t);
    }
  }

  void CopyFrom(const char* buffer) {
    std::memcpy(&feature, buffer, sizeof(feature));
    buffer += sizeof(feature);
    std::memcpy(&left_count, buffer, sizeof(left_count));
    buffer += sizeof(left_count);
    std::memcpy(&right_count, buffer, sizeof(right_count));
    buffer += sizeof(right_count);
    std::memcpy(&gain, buffer, sizeof(gain));
    buffer += sizeof(gain);
    std::memcpy(&threshold, buffer, sizeof(threshold));
    buffer += sizeof(threshold);
    std::memcpy(&left_output, buffer, sizeof(left_output));
    buffer += sizeof(left_output);
    std::memcpy(&right_output, buffer, sizeof(right_output));
    buffer += sizeof(right_output);
    std::memcpy(&left_sum_gradient, buffer, sizeof(left_sum_gradient));
    buffer += sizeof(left_sum_gradient);
    std::memcpy(&left_sum_hessian, buffer, sizeof(left_sum_hessian));
    buffer += sizeof(left_sum_hessian);
    std::memcpy(&right_sum_gradient, buffer, sizeof(right_sum_gradient));
    buffer += sizeof(right_sum_gradient);
    std::memcpy(&right_sum_hessian, buffer, sizeof(right_sum_hessian));
    buffer += sizeof(right_sum_hessian);
    std::memcpy(&default_left, buffer, sizeof(default_left));
    buffer += sizeof(default_left);
    cat_threshold.resize(num_cat_threshold);
    for (int i = 0; i < num_cat_threshold; ++i) {
      std::memcpy(&cat_threshold[i], buffer, sizeof(uint32_t));
      buffer += sizeof(uint32_t);
    }
  }

  inline void Reset() {
    // initialize with -1 and -inf gain
    feature = -1;
    gain = kMinScore;
  }

  inline bool operator > (const SplitInfo& si) const {
    double local_gain = this->gain;
    double other_gain = si.gain;
    // replace nan with -inf
    if (local_gain == NAN) {
      local_gain = kMinScore;
    }
    // replace nan with -inf
    if (other_gain == NAN) {
      other_gain = kMinScore;
    }
    int local_feature = this->feature;
    int other_feature = si.feature;
    // replace -1 with max int
    if (local_feature == -1) {
      local_feature = INT32_MAX;
    }
    // replace -1 with max int
    if (other_feature == -1) {
      other_feature = INT32_MAX;
    }
    if (local_gain != other_gain) {
      return local_gain > other_gain;
    } else {
      // if same gain, use smaller feature
      return local_feature < other_feature;
    }
  }

  inline bool operator == (const SplitInfo& si) const {
    double local_gain = this->gain;
    double other_gain = si.gain;
    // replace nan with -inf
    if (local_gain == NAN) {
      local_gain = kMinScore;
    }
    // replace nan with -inf
    if (other_gain == NAN) {
      other_gain = kMinScore;
    }
    int local_feature = this->feature;
    int other_feature = si.feature;
    // replace -1 with max int
    if (local_feature == -1) {
      local_feature = INT32_MAX;
    }
    // replace -1 with max int
    if (other_feature == -1) {
      other_feature = INT32_MAX;
    }
    if (local_gain != other_gain) {
      return local_gain == other_gain;
    } else {
      // if same gain, use smaller feature
      return local_feature == other_feature;
    }
  }

};

struct LightSplitInfo {
public:
  /*! \brief Feature index */
  int feature;
  /*! \brief Split gain */
  double gain;
  /*! \brief Left number of data after split */
  data_size_t left_count;
  /*! \brief Right number of data after split */
  data_size_t right_count;

  LightSplitInfo() {
    // initialize with -1 and -inf gain
    feature = -1;
    gain = kMinScore;
  }

  inline void Reset() {
    // initialize with -1 and -inf gain
    feature = -1;
    gain = kMinScore;
  }

  void CopyFrom(const SplitInfo& other) {
    feature = other.feature;
    gain = other.gain;
    left_count = other.left_count;
    right_count = other.right_count;
  }

  void CopyFrom(const char* buffer) {
    std::memcpy(&feature, buffer, sizeof(feature));
    buffer += sizeof(feature);
    std::memcpy(&left_count, buffer, sizeof(left_count));
    buffer += sizeof(left_count);
    std::memcpy(&right_count, buffer, sizeof(right_count));
    buffer += sizeof(right_count);
    std::memcpy(&gain, buffer, sizeof(gain));
    buffer += sizeof(gain);
  }

  inline bool operator > (const LightSplitInfo& si) const {
    double local_gain = this->gain;
    double other_gain = si.gain;
    // replace nan with -inf
    if (local_gain == NAN) {
      local_gain = kMinScore;
    }
    // replace nan with -inf
    if (other_gain == NAN) {
      other_gain = kMinScore;
    }
    int local_feature = this->feature;
    int other_feature = si.feature;
    // replace -1 with max int
    if (local_feature == -1) {
      local_feature = INT32_MAX;
    }
    // replace -1 with max int
    if (other_feature == -1) {
      other_feature = INT32_MAX;
    }
    if (local_gain != other_gain) {
      return local_gain > other_gain;
    } else {
      // if same gain, use smaller feature
      return local_feature < other_feature;
    }
  }

  inline bool operator == (const LightSplitInfo& si) const {
    double local_gain = this->gain;
    double other_gain = si.gain;
    // replace nan with -inf
    if (local_gain == NAN) {
      local_gain = kMinScore;
    }
    // replace nan with -inf
    if (other_gain == NAN) {
      other_gain = kMinScore;
    }
    int local_feature = this->feature;
    int other_feature = si.feature;
    // replace -1 with max int
    if (local_feature == -1) {
      local_feature = INT32_MAX;
    }
    // replace -1 with max int
    if (other_feature == -1) {
      other_feature = INT32_MAX;
    }
    if (local_gain != other_gain) {
      return local_gain == other_gain;
    } else {
      // if same gain, use smaller feature
      return local_feature == other_feature;
    }
  }

};

}  // namespace LightGBM
#endif   // LightGBM_TREELEARNER_SPLIT_INFO_HPP_
