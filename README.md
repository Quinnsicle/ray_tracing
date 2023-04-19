This is my journey through Ray Tracing in One Weekend

Note 1:
He uses the ppm file format to save the images and mentions using the stb_image library for other formats so I created an image class in lib/image.hpp that write to different file formats. Currently the only file formats I support are ppm and jpg but I could easily use stb_image to add more.
