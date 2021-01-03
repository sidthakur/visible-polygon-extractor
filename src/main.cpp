#include "visible-polygon-extractor.h"

int main(int argc, char *argv[]) {
    std::string path_to_output_dir;
    std::string ros_bag_file;

    boost::program_options::options_description description{"Allowed options"};
    description.add_options()
            ("help,h", "Print usage message")
            ("ros-bag-file,i",
             boost::program_options::value<std::string>(&ros_bag_file)->default_value(
                     "/Users/sthakur/workspace/toy-problem-data/20210102_181520.bag"),
             "Input: ROS Bag file")
            ("path-to-output-dir,o",
             boost::program_options::value<std::string>(&path_to_output_dir)->default_value(
                     "/Users/sthakur/workspace/toy-problem-data/output"),
             "Output: Path to output dir");
    boost::program_options::variables_map vm;
    boost::program_options::store(parse_command_line(argc, argv, description), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << description << std::endl;
        return EXIT_SUCCESS;
    }

    vpe::DebugOutputWriter debug_output_writer(path_to_output_dir);
    debug_output_writer.InitializeOutputDirectory();

    rs2::pipeline pipeline;
    rs2::config config;
    vpe::InitializeConfig(&config, ros_bag_file);

    rs2::pipeline_profile profile = pipeline.start(config);
    rs2::playback playback = profile.get_device().as<rs2::playback>();

    // Wait for first frame to allow the update of playback status
    pipeline.wait_for_frames();

    while (playback.current_status() == RS2_PLAYBACK_STATUS_PLAYING)
        vpe::HandleNewFrames(&pipeline, path_to_output_dir, debug_output_writer);

    return EXIT_SUCCESS;
}
