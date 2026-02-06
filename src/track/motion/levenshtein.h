/***
 * @Author: lwn
 * @Date: 2024-04-24 16:53:23
 * @LastEditors: lwn
 * @LastEditTime: 2024-04-24 17:32:29
 * @FilePath: \FalconEye\include\track\motion\levenshtein.h
 * @Description:
 */
#include <string>
#include <vector>

namespace sonli
{
namespace Levenshtein
{
/***
 * @description: Calculates the minimum number of insertions, deletions, and substitutions
                 required to change one sequence into the other according to Levenshtein with custom
                 costs for insertion, deletion and substitution
 * @param {string&} s1:  First string to compare.
 * @param {string&} s2:  Second string to compare.
 * @param {vector<int>} weights: [int, int, int] or empty, optional
                                The weights for the three operations in the form
                                (insertion, deletion, substitution). Default is (1, 1, 1),
                                which gives all three operations a weight of 1.
 * @param {int} score_cutoff:  optional
                                Maximum distance between s1 and s2, that is
                                considered as a result. If the distance is bigger than score_cutoff,
                                score_cutoff + 1 is returned instead. Default is None, which deactivates
                                this behaviour.
 * @return {*}
 */
int
distance(const std::string &s1, const std::string &s2, const std::vector<int> weights = {1, 1, 1},
         int score_cutoff = 0);

/***
 * @description: Calculates the levenshtein similarity in the range [max, 0] using custom
                 costs for insertion, deletion and substitution.

                This is calculated as ``max - distance``, where max is the maximal possible
                Levenshtein distance given the lengths of the sequences s1/s2 and the weights.
 * @param {string&} s1:  First string to compare.
 * @param {string&} s2:  Second string to compare.
 * @param {vector<int>} weights: [int, int, int] or empty, optional
                                The weights for the three operations in the form
                                (insertion, deletion, substitution). Default is (1, 1, 1),
                                which gives all three operations a weight of 1.
 * @param {int} score_cutoff:  optional
                                Maximum distance between s1 and s2, that is
                                considered as a result. If the distance is bigger than score_cutoff,
                                score_cutoff + 1 is returned instead. Default is None, which deactivates
                                this behaviour.
 * @return {*}
 */
int
similarity(const std::string &s1, const std::string &s2, const std::vector<int> weights = {1, 1, 1},
           int score_cutoff = 0);

/***
 * @description:  Calculates a normalized levenshtein distance in the range [1, 0] using custom
                  costs for insertion, deletion and substitution.

                  This is calculated as ``distance / max``, where max is the maximal possible
                  Levenshtein distance given the lengths of the sequences s1/s2 and the weights.
 * @param {string&} s1:  First string to compare.
 * @param {string&} s2:  Second string to compare.
 * @param {vector<int>} weights: [int, int, int] or empty, optional
                                The weights for the three operations in the form
                                (insertion, deletion, substitution). Default is (1, 1, 1),
                                which gives all three operations a weight of 1.
 * @param {int} score_cutoff:  optional
                                Maximum distance between s1 and s2, that is
                                considered as a result. If the distance is bigger than score_cutoff,
                                score_cutoff + 1 is returned instead. Default is None, which deactivates
                                this behaviour.
 * @return {*}
 */
float
normalized_distance(const std::string &s1, const std::string &s2, const std::vector<int> weights = {1, 1, 1},
                    int score_cutoff = 0);

/***
 * @description:  Calculates a normalized levenshtein similarity in the range [0, 1] using custom
                  costs for insertion, deletion and substitution.

                  This is calculated as ``1 - normalized_distance``
 * @param {string&} s1:  First string to compare.
 * @param {string&} s2:  Second string to compare.
 * @param {vector<int>} weights: [int, int, int] or empty, optional
                                The weights for the three operations in the form
                                (insertion, deletion, substitution). Default is (1, 1, 1),
                                which gives all three operations a weight of 1.
 * @param {int} score_cutoff:  optional
                                Maximum distance between s1 and s2, that is
                                considered as a result. If the distance is bigger than score_cutoff,
                                score_cutoff + 1 is returned instead. Default is None, which deactivates
                                this behaviour.
 * @return {*}
 */
float
normalized_similarity(const std::string &s1, const std::string &s2, const std::vector<int> weights = {1, 1, 1},
                      int score_cutoff = 0);
} // namespace Levenshtein
} // namespace sonli