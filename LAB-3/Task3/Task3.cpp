#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <cmath> // Needed for abs()

// Structure to hold the data needed by the callback
struct MouseCallbackData {
    const cv::Mat* original_image_ptr; // Pointer to the *unmodified* source image
    cv::Mat* display_image_ptr;      // Pointer to the image being *shown* (will become the mask)
    std::string window_name;         // Name of the window to update
};

void click(int event, int x, int y, int flags, void* userdata);

int main(int argc, char** argv) {

    std::string filename = "Robocup.jpg";
    if (argc >= 2) {
        filename = argv[1];
    } else {
        std::cout << "No image filename provided. Trying default 'Robocup.jpg'\n";
    }

    cv::Mat original_image = cv::imread(filename); // Load the original image
    if (original_image.empty()) {
        std::cout << "File name '" << filename << "' is wrong or the file does not exist\n";
        return -1;
    }

    // Create the image that will be displayed and modified by the callback
    cv::Mat display_image = original_image.clone();
    std::string window_title = "Image";

    // Prepare the data structure to pass to the callback
    MouseCallbackData cb_data;
    cb_data.original_image_ptr = &original_image; // Address of the original
    cb_data.display_image_ptr = &display_image;    // Address of the one to display/modify
    cb_data.window_name = window_title;

    cv::namedWindow(window_title);
    // Pass the address of our data structure as userdata
    cv::setMouseCallback(window_title, click, &cb_data);

    cv::imshow(window_title, display_image); // Initial display (shows original at first)
    cv::waitKey(0);
    cv::destroyAllWindows(); // Clean up window
    return 0;
}

// The mouse callback function
void click(int event, int x, int y, int flags, void* userdata) {
    // Process only left button down events
    if (event == cv::EVENT_LBUTTONDOWN) {

        // Cast the generic void* back to our specific structure type
        MouseCallbackData* data = static_cast<MouseCallbackData*>(userdata);

        // Make sure the pointers in the data structure are valid (basic check)
        if (!data || !data->original_image_ptr || !data->display_image_ptr) {
             std::cerr << "Error: Invalid userdata passed to callback." << std::endl;
             return;
        }

        // Get direct references/pointers for convenience
        const cv::Mat& original_img = *(data->original_image_ptr);
        cv::Mat& display_img = *(data->display_image_ptr); // We will modify this one
        const std::string& window_name = data->window_name;

        // Ensure coordinates are within image bounds (using original image dimensions)
        if (x < 0 || x >= original_img.cols || y < 0 || y >= original_img.rows) {
            std::cout << "Clicked outside image bounds." << std::endl;
            return;
        }

        // Get the clicked pixel color FROM THE ORIGINAL UNMODIFIED IMAGE
        cv::Vec3b clicked_pixel_color = original_img.at<cv::Vec3b>(y, x);
        std::cout << "Clicked Color (BGR): "
                  << (int)clicked_pixel_color[0] << ", "
                  << (int)clicked_pixel_color[1] << ", "
                  << (int)clicked_pixel_color[2] << std::endl;

        int tolerance = 25;

        // Create a new mask image, initialized to black.
        // Use the dimensions of the original image.
        cv::Mat mask = cv::Mat::zeros(original_img.size(), original_img.type());

        // Iterate through the ORIGINAL image to build the mask
        for (int j = 0; j < original_img.rows; j++) { // Iterate rows (y)
            for (int i = 0; i < original_img.cols; i++) { // Iterate columns (x)

                // Get the current pixel color FROM THE ORIGINAL UNMODIFIED IMAGE
                cv::Vec3b current_pixel_color = original_img.at<cv::Vec3b>(j, i);

                // Check if *each* channel is within the tolerance range
                bool blue_ok  = std::abs(current_pixel_color[0] - clicked_pixel_color[0]) <= tolerance;
                bool green_ok = std::abs(current_pixel_color[1] - clicked_pixel_color[1]) <= tolerance;
                bool red_ok   = std::abs(current_pixel_color[2] - clicked_pixel_color[2]) <= tolerance;

                // If all channels are within tolerance, make the pixel white in the mask
                if (blue_ok && green_ok && red_ok) {
                    mask.at<cv::Vec3b>(j, i) = cv::Vec3b(255, 255, 255);
                }
                // No 'else' needed because the mask was initialized to black
            }
        }

        // Update the image being displayed (pointed to by data->display_image_ptr)
        // with the newly created mask.
        display_img = mask; // This modifies the 'display_image' variable in main()

        // Refresh the display in the window
        cv::imshow(window_name, display_img); // Use the updated display_img
        std::cout << "Mask updated and displayed." << std::endl;
    }
}