/*
*  Jan Prokaj
*  February 20, 2005
*  based on libsift-1.6 -- Sebastian Nowozin (nowozin@cs.tu-berlin.de)
*/

#ifndef SIFTCONSTANTS_H
#define SIFTCONSTANTS_H

#define START_SCALE 1.0
#define START_SIGMA 1.6
#define OCTAVE_LIMIT 6 /* how many octaves we are considering */
#define IMAGES_PER_OCTAVE 3 /* number of useful images for extrema detection per octave */
#define MIN_SIZE 24 /* minimum useful size that we can work with (was 32) */
#define SKIP_OCTAVES 1 /* sets the number of lower octaves to skip */
#define MAX_ADJ_STEPS 4 /* number of times to try to localize a point */
#define EDGE_RATIO 10.0 /* used in filtering points that are along an edge, page 12 */
#define CONTRAST_THRESH 0.03/* used in filtering points, page 11  (lowe=0.03) */
#define PEAK_PERCENT 0.60 /* determines the number of multiple orientations (lowe=0.80)*/
#define CAP 0.2 /* reduces the effect of illumination (0.2), page 16 */
#define M_PI 3.1415926

#endif
