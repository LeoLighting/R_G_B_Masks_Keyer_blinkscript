// Define a custom lerp function for linear interpolation
float lerp(float start, float end, float factor) 
{
    return start + factor * (end - start);
}

kernel R_G_B_Masks_keyer: ImageComputationKernel<ePixelWise>
{
    // Define the input and output images
    Image<eRead, eAccessPoint, eEdgeClamped> B; // Input image
    Image<eWrite, eAccessPoint> out;           // Output image

    param:
        // R mask = bright values
        // G mask = medium values
        // B mask = dark values

        float r_h;  // Red mask high value
        float r_l;  // Red mask low value
        float g_h;  // Green mask high value
        float g_l;  // Green mask low value
        float b_h;  // Blue mask high value
        float b_l;  // Blue mask low value
        bool invert;// Boolean parameter to invert

    void define()
    {
        defineParam(r_h, "R mask max", 0.5f);
        defineParam(r_l, "R mask min", 0.5f);
        defineParam(g_h, "G mask max", 0.5f);
        defineParam(g_l, "G mask min", 0.5f);
        defineParam(b_h, "B mask max", 0.5f);
        defineParam(b_l, "B mask min", 0.5f);
        defineParam(invert, "Invert Mask", false);
    }

    void process()
    {
        // Read the pixel value from the input image B
        float inputPixel = B(0);

        float r_range = max(r_h - r_l, 1e-5f);
        float g_range = max(g_h - g_l, 1e-5f);
        float b_range = max(b_h - b_l, 1e-5f);

        // Determine the interpolation factor based on the input pixel value
        // Normalize the factor between minValue and maxValue
        float RMask_factor = clamp((inputPixel - r_l) / r_range, 0.0f, 1.0f);
        float GMask_factor = clamp((inputPixel - g_l) / g_range, 0.0f, 1.0f);
        float BMask_factor = clamp((inputPixel - b_l) / b_range, 0.0f, 1.0f);

        // Ensure out-of-bounds values are handled properly
        float R_Key = (inputPixel >= r_l && inputPixel <= r_h)
            ? lerp(1.0f, 0.0f, invert ? RMask_factor : 1.0f - RMask_factor)
            : 0.0f;

        float G_Key = (inputPixel >= g_l && inputPixel <= g_h)
            ? lerp(1.0f, 0.0f, invert ? GMask_factor : 1.0f - GMask_factor)
            : 0.0f;

        float B_Key = (inputPixel >= b_l && inputPixel <= b_h)
            ? lerp(1.0f, 0.0f, invert ? BMask_factor : 1.0f - BMask_factor)
            : 0.0f;

        // Set the output pixel values for each channel
        out(0) = R_Key;  // Red channel
        out(1) = G_Key;  // Green channel
        out(2) = B_Key;  // Blue channel
        out(3) = B(3);              // Alpha channel from the original input
    }
};
