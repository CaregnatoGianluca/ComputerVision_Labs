#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm> // Needed for std::min

// Structure to hold the data needed by the callback
struct MouseCallbackData {
    const cv::Mat* original_bgr_image_ptr; // Pointer to the original BGR image (for display and getting clicked BGR)
    const cv::Mat* original_hsv_image_ptr; // Pointer to the converted HSV image (for comparison)
    cv::Mat* display_image_ptr;          // Pointer to the image being shown (will become the mask)
    std::string window_name;             // Name of the window to update (the mask window)
};

void click(int event, int x, int y, int flags, void* userdata);
// Helper function for circular hue difference
int hueDifference(int h1, int h2);

int main(int argc, char** argv) {

    std::string filename = "Robocup.jpg";

    cv::Mat original_image = cv::imread(filename); // Load the original BGR image
    if (original_image.empty()) {
        std::cout << "File name '" << filename << "' is wrong or the file does not exist\n";
        return -1;
    }

    //Convert to HSV 
    cv::Mat hsv_image;
    cv::cvtColor(original_image, hsv_image, cv::COLOR_BGR2HSV);


    // This image will hold the black/white mask output
    cv::Mat display_image = cv::Mat::zeros(original_image.size(), original_image.type());
    std::string mask_window_title = "HSV Mask";
    std::string original_window_title = "Image";


    // Prepare the data structure to pass to the callback
    MouseCallbackData cb_data;
    cb_data.original_bgr_image_ptr = &original_image; // Pass pointer to original BGR
    cb_data.original_hsv_image_ptr = &hsv_image;      // Pass pointer to converted HSV
    cb_data.display_image_ptr = &display_image;       // Pass pointer to the mask image
    cb_data.window_name = mask_window_title;          // Name of the mask window

    // Create windows
    cv::namedWindow(original_window_title);
    cv::namedWindow(mask_window_title);

    cv::setMouseCallback(original_window_title, click, &cb_data);

    // Initial display
    cv::imshow(original_window_title, original_image); // Show original BGR
    cv::imshow(mask_window_title, display_image);      // Show the initially empty mask


    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}

/*The point of doing this function is that hue is circular, so 178 is close to 2.*/
int hueDifference(int h1, int h2) {
    int diff = std::abs(h1 - h2);
    return std::min(diff, 180 - diff); 
}

// The mouse callback function
void click(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {

        MouseCallbackData* data = static_cast<MouseCallbackData*>(userdata);
        if (!data || !data->original_bgr_image_ptr || !data->original_hsv_image_ptr || !data->display_image_ptr) {
             std::cerr << "Error: Invalid userdata passed to callback." << std::endl;
             return;
        }

        const cv::Mat& original_bgr_img = *(data->original_bgr_image_ptr);
        const cv::Mat& hsv_img = *(data->original_hsv_image_ptr); // Use the pre-converted HSV image
        cv::Mat& display_img = *(data->display_image_ptr);
        const std::string& window_name = data->window_name;


        //Get Clicked Color in HSV 
        cv::Vec3b clicked_bgr_pixel = original_bgr_img.at<cv::Vec3b>(y, x);

        //Convert *this single pixel* to HSV to get the target HSV values
        cv::Mat clicked_pixel_mat(1, 1, CV_8UC3); // Create 1x1 matrix
        clicked_pixel_mat.at<cv::Vec3b>(0, 0) = clicked_bgr_pixel;
        cv::cvtColor(clicked_pixel_mat, clicked_pixel_mat, cv::COLOR_BGR2HSV);
        cv::Vec3b clicked_hsv_pixel = clicked_pixel_mat.at<cv::Vec3b>(0, 0);

        int clicked_h = clicked_hsv_pixel[0]; // Hue (0-179)
        int clicked_s = clicked_hsv_pixel[1]; // Saturation (0-255)
        int clicked_v = clicked_hsv_pixel[2]; // Value (0-255)

        std::cout << "Clicked BGR: " << (int)clicked_bgr_pixel[0] << "," << (int)clicked_bgr_pixel[1] << "," << (int)clicked_bgr_pixel[2]
                  << " -> Target HSV: " << clicked_h << "," << clicked_s << "," << clicked_v << std::endl;

        int h_tolerance = 10;
        int s_tolerance = 135;
        int v_tolerance = 135;


        cv::Mat mask = cv::Mat::zeros(hsv_img.size(), CV_8UC3); // Output mask is BGR

        //Iterate through the PRE-CONVERTED HSV image
        for (int j = 0; j < hsv_img.rows; j++) {
            for (int i = 0; i < hsv_img.cols; i++) {

                // Get the current pixel's HSV values directly from the hsv_img
                cv::Vec3b current_hsv_pixel = hsv_img.at<cv::Vec3b>(j, i);
                int current_h = current_hsv_pixel[0];
                int current_s = current_hsv_pixel[1];
                int current_v = current_hsv_pixel[2];

                //Compare HSV values with tolerance
                bool hue_ok   = hueDifference(current_h, clicked_h) <= h_tolerance;
                bool sat_ok   = std::abs(current_s - clicked_s) <= s_tolerance;
                bool value_ok = std::abs(current_v - clicked_v) <= v_tolerance;

                if (hue_ok && sat_ok && value_ok) {
                    mask.at<cv::Vec3b>(j, i) = cv::Vec3b(255, 255, 255);
                }
            }
        }

        // Refresh the mask window display
        cv::imshow(window_name, mask);
    }
}