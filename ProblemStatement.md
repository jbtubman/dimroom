# C++ Coding Example

## Overview

The context in which to consider this assessment is that we plan to develop a system
that can be ultimately be used to both manage and edit digital photos. This would be
akin to Adobe’s Lightroom product.

Users would be able import, tag, search for, edit and export images from within the
application.

The requirements outlined below focus on the handling of photo metadata; however, it
is important to have an awareness of the needs of the system as a whole. Examples
are that the images could come from different sources (scanned physical photographs,
digital camera, cell phone) and that it will be possible to adjust at least some aspects of
the metadata within the application.

The goal of the assessment is to see how the system is architected, as well as see your
coding style, and the technical accuracy of the coding itself.

## Specifications

Given a list of images with tags (see associated file, Image Library Samples.csv) create
an application that can:

* Load the images’ metadata into memory
  * You can choose to gather equivalent information from real images from a
directory as well; however, it is not required
* Find files based on their properties
  * Search for images using up to N tag-value pairs
  * Search for images that exist within a geocoordinate space represented by
a polygon of lat-long points
  * Only AND operations need to be supported within the search criteria
  * Example searches:
    * Files that are tagged as favorites and have a DPI greater than 200
    * Files within the following polygon
      * (51.129, -114.010), (50.742, -113.948), (50.748, -113.867)
* Take search criteria as an input

## Considerations

* There are some standard pieces of metadata that each image will have, e.g.
image size, and dimensions
* The number of metadata fields populated for any given image are unknown and
each image type can contain different fields
* Metadata fields may have multiple tags within them - see User Tags in the
provided CSV file
* The system should be expected to handle the entire contents of a user’s photo
library. For some people, this will be in the order of hundreds of thousands of
images

## Requirements

* Written using C++
* Must compile
  * An IDE project is sufficient
* Command line interface for passing the search criteria
  * You can go further than this if you wish, as long as search criteria can be entered
* Basic documentation to describe how to run the application, and provide search criteria
* A brief description of the approach you took, and notes on anything you would
have done differently had this been commercial project
