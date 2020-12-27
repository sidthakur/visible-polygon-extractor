#include "rs-helpers.h"

int main() {
    rs2::pipeline pipeline;
    rs2::config config;
    vpe::InitializeConfig(&config);

    rs2::pipeline_profile profile = pipeline.start(config);
    rs2::playback playback = profile.get_device().as<rs2::playback>();

    // Wait for first frame to allow the update of playback status
    pipeline.wait_for_frames();

    while (playback.current_status() == RS2_PLAYBACK_STATUS_PLAYING) {
        vpe::HandleNewFrames(&pipeline);
    }
    return 0;
}
