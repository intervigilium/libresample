/* resample.c - sampling rate conversion subroutines
 *
 * Original version available at the
 * Digital Audio Resampling Home Page located at
 * http://ccrma.stanford.edu/~jos/resample/.
 *
 * Modified for use on Android by Ethan Chen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "resample-interface.h"
#include "resample.h"
#include <stdlib.h>
#include <string.h>
#include <android/log.h>

static int num_channels;
static struct rs_data **rs;

static void downMix(short *out, short *pcm_left, short *pcm_right, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		out[i] = (pcm_left[i] / 2 + pcm_right[i] / 2);
	}
}

JNIEXPORT void JNICALL Java_net_sourceforge_resample_Resample_downmix
    (JNIEnv * env, jclass class, jshortArray outputBuffer,
     jshortArray inputLeft, jshortArray inputRight, jint numSamples) {
	short *input_left, *input_right, *output_buf;

	output_buf =
	    (short *)(*env)->GetPrimitiveArrayCritical(env, outputBuffer, 0);
	input_left =
	    (short *)(*env)->GetPrimitiveArrayCritical(env, inputLeft, 0);
	input_right =
	    (short *)(*env)->GetPrimitiveArrayCritical(env, inputRight, 0);

	downMix(output_buf, input_left, input_right, numSamples);

	/* dont bother updating left/right buffers */
	(*env)->ReleasePrimitiveArrayCritical(env, inputLeft, input_left,
					      JNI_ABORT);
	(*env)->ReleasePrimitiveArrayCritical(env, inputRight, input_right,
					      JNI_ABORT);
	/* update the output buffer */
	(*env)->ReleasePrimitiveArrayCritical(env, outputBuffer, output_buf, 0);
}

JNIEXPORT jint JNICALL Java_net_sourceforge_resample_Resample_resample
  (JNIEnv *env, jclass class, jdouble factor, jshortArray inputBuffer, jshortArray outputBuffer, jint numSamples)
{
    int num;
    short *in_buf, *out_buf;

    in_buf = (short *)(*env)->GetPrimitiveArrayCritical(env, inputBuffer, 0);
    out_buf = (short *)(*env)->GetPrimitiveArrayCritical(env, outputBuffer, 0);

    num = resample_simple(factor, in_buf, out_buf, numSamples);

    (*env)->ReleasePrimitiveArrayCritical(env, inputBuffer, in_buf, JNI_ABORT);
    (*env)->ReleasePrimitiveArrayCritical(env, outputBuffer, out_buf, 0);

    return num;
}

JNIEXPORT void JNICALL Java_net_sourceforge_resample_Resample_initialize
    (JNIEnv * env, jclass class, jint inputRate, jint outputRate,
     jint bufferSize, jint channels) {
	int i;
	if (!rs) {
		num_channels = channels;
		rs = calloc(num_channels, sizeof(struct rs_data *));
		for (i = 0; i < num_channels; i++) {
			rs[i] =
			    resample_init(inputRate, outputRate, bufferSize);
		}
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, "libresample.so",
				    "Unable to initialize libresample");
	}
}

JNIEXPORT jdouble JNICALL Java_net_sourceforge_resample_Resample_getFactor
    (JNIEnv * env, jclass class) {
	return rs[net_sourceforge_resample_Resample_CHANNEL_MONO]->factor;
}

JNIEXPORT jint JNICALL Java_net_sourceforge_resample_Resample_process
    (JNIEnv * env, jclass class, jshortArray inputBuffer,
     jshortArray outputBuffer, jint channel, jboolean isLast) {
	int i, in_len, out_len, res;
	short *in_buf, *out_buf;

	if (!rs) {
		__android_log_print(ANDROID_LOG_DEBUG, "libresample.so",
				    "No suitable resample instance found!");
		return -1;
	}

	if (channel >= net_sourceforge_resample_Resample_MAX_CHANNELS) {
		__android_log_print(ANDROID_LOG_DEBUG, "libresample.so",
				    "Resample supports stereo, mono only!");
		return -1;
	}

	in_len = (*env)->GetArrayLength(env, inputBuffer);
	out_len = (*env)->GetArrayLength(env, outputBuffer);
	in_buf =
	    (short *)(*env)->GetPrimitiveArrayCritical(env, inputBuffer, 0);
	out_buf =
	    (short *)(*env)->GetPrimitiveArrayCritical(env, outputBuffer, 0);

	res = resample(rs[channel], in_buf, in_len, out_buf, out_len, isLast);
	/* don't bother updating input buffer */
	(*env)->ReleasePrimitiveArrayCritical(env, inputBuffer, in_buf,
					      JNI_ABORT);
	/* do update the output buffer */
	(*env)->ReleasePrimitiveArrayCritical(env, outputBuffer, in_buf, 0);

	return res;
}

JNIEXPORT void JNICALL Java_net_sourceforge_resample_Resample_close
    (JNIEnv * env, jclass class) {
	int i;
	if (rs) {
		for (i = 0; i < num_channels; i++) {
			resample_close(rs[i]);
		}
		free(rs);
		rs = NULL;
		num_channels = 0;
	}
}
