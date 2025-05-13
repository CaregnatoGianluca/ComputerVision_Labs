#include "../include/panoramic_utils.h" // For imagesLoader - ensure this path is correct
#include "../include/stitcher.h"        // For Stitcher class - ensure this path is correct

#include <opencv2/highgui.hpp> // For imwrite, imshow (optional)
#include <iostream>
#include <vector>
#include <string>

int main(){
    // Base path to the datasets folder
    std::string base_image_path = "../Images/"; // Adjust if your "Images" folder is elsewhere

    // Configure datasets: {directory_path_component, dataset_identifier}
    // dataset_identifier is used for FoV selection ("dolomites" vs other) and output filenames.
    std::vector<std::pair<std::string, std::string>> dataset_configs = {
        {"dataset_dolomites/dolomites", "dolomites"}, // dir_component, id
        {"dataset_kitchen/kitchen", "kitchen"},       // dir_component, id
        {"dataset_lab/data", "lab"}                   // dir_component, id
        // Add other datasets if needed
    };

    // Create Stitcher instance
    Stitcher stitcher(3.0, 5.0); // Values can be tuned

    for (const auto& config : dataset_configs) {
        std::string dataset_dir_component = config.first;  // e.g., "dataset_dolomites/dolomites"
        std::string dataset_id = config.second; // <<--- CORRECTED LINE (use the id: "dolomites", "kitchen", etc.)

        std::string full_dataset_path = base_image_path + dataset_dir_component;

        std::cout << "\nProcessing dataset ID: " << dataset_id << " from path: " << full_dataset_path << std::endl;

        std::vector<cv::Mat> images = imagesLoader(full_dataset_path);

        // It's good practice to print the path used for loading
        std::cout << "Attempting to load images from: " << full_dataset_path << std::endl;

        if (images.empty()) {
            std::cerr << "No images loaded for dataset ID: " << dataset_id << ". Skipping." << std::endl;
            continue;
        }
        std::cout << "Loaded " << images.size() << " images." << std::endl;

        // Perform stitching, passing the CORRECT dataset_id for FoV selection
        cv::Mat panorama = stitcher.stitch(images, dataset_id);

        if (!panorama.empty()) {
            // Use the CORRECT dataset_id for the filename
            std::string output_filename = "panorama_" + dataset_id + ".jpg";
            if (cv::imwrite(output_filename, panorama)) {
                std::cout << "Panorama saved to " << output_filename << std::endl;
            } else {
                std::cerr << "Failed to save panorama: " << output_filename << std::endl;
            }

            // Optionally display the panorama with the CORRECT title
            cv::imshow("Panorama: " + dataset_id, panorama);
            std::cout << "Displaying final panorama for " << dataset_id << ". Press any key..." << std::endl;
            cv::waitKey(0);
        } else {
            std::cout << "Failed to generate panorama for dataset ID: " << dataset_id << std::endl;
        }
    }

    cv::destroyAllWindows();
    return 0;
}