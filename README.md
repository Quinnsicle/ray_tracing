This is my journey through Ray Tracing in One Weekend

Here is an image of the current output of the ray tracer

![Current image generated by the ray tracer](/img/random_spheres.jpg)

---
## Instructions
*This project depends on the stb library for the jpg writer*

To build and run simply use 
`make test`

---
### My Notes
- He uses the ppm file format to save the images and mentions using the stb_image library for other formats so I created an image class in lib/image.hpp that write to different file formats. Currently the only file formats I support are ppm and jpg but I could easily use stb_image to add more.
- The strategy for hemispherical rendering is useful for creating rays pointing outward or inward from the hit point which could be useful for subsurface scattering. I'll have to do some research to see if this initial thought is correct...
- I'm trying to implement multithreading for the rendering loops. I want each line of pixels to be rendered by a different thread. I keep getting bad images. I have a mutex guard on the vector that holds all the pixels of the image, but I realized that the order of the pixels isn't being preserved since each thread inserts it's  pixel at different times. I could find a way to preserve the order while inserting but that would probably slow things down. I could map each pixel to it's x,y pair, then I'd have to reorder the pixels or rewrite the image write function to account for this new structure. I also could reserve enough space for all the pixels for the vector and insert each pixel directly where it belongs in the vector so that the order is preserved and I don't have to change the data structure. I used this last method and it works but the image is upside down for some reason. This is due to the origin being at the bottom left instead of the top left like I thought. I used this equation, i + (j * image_width), to calculate the insert location in the pixels vector. That equation was for a coordinate system with origin at the top left. I modified the equation to (image_height - j - 1) * image_width + i. I could also change the Camera class to have an origin at the top left, I'd like to do this eventually since it would bring more consistency to the rendering loop. I've kept most of the failure images in the img/threading_fails directory. 4 is my favorite which was caused by me naively multiplying i and j for the pixels index.
- I've changed the Camera class to use the top left corner as a starting point instead of the lower left corner. This simplifies the rendering loop solutions I came up with to add multithreading.


### Performance

- Single-threaded time: 107021 ms
- Multi-threaded time: 35014.3 ms
- VH time: 27063 ms
- System: i7-9850H CPU @ 2.60GHz
- Number of threads: 12
- Threading Improvement: 3.0565 times faster!
- BVH Improvement: 1.29381 times faster!
