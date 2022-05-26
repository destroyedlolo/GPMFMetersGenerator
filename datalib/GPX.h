/* GPX
 * 	Impersonates GPX data and extended Story
 */

#ifndef GPX_H
#define GPX_H

#include "GPSCoordinate.h"
#include "samplesCollection.h"

#include <ctime>

struct GpxData : public GPSCoordinate {
	GpxData(){};
	GpxData(
		double alatitude, double alongitude,
		double aaltitude,
		time_t asample_time,
		double adistance = 0
	) : GPSCoordinate(alatitude, alongitude, aaltitude, asample_time, adistance) {
	}
};

struct StoryVideo : public std::string {
	int start;
	int end;

	StoryVideo( const char *name, int astart, int aend ) : 
		std::string(name), start(astart), end(aend) {}

		/* check if given index is part of the video
		 * -1 : before
		 *  0 : part of it
		 *  1 : after
		 */
	int whithin( int idx ){
		if(idx < this->start)
			return -1;
		else if(idx > this->end)
			return 1;
		else
			return 0;
	}
};

class GPX : public samplesCollection<GpxData> {
	std::vector<StoryVideo> videos;
	int current_video_idx;	// Current video index

	void readGPX( const char * );
	void readStory( const char * );

	void updMinMax( GpxData & );

public:
		/* Read hiking traces from a GPX file or from a Story file
		 * -> file : file to be read
		 * -> story : true if it'a a story file
		 */
	GPX( const char *, bool story = false );

	void Dump( void );

		/* Check if given coordinate is close to the footprint of the hiking.
		 * It's a very quick and dirty test ... but enough to check a video
		 * has been took in the same area
		 * -> proximity_threshold in degree
		 */
	bool sameArea( GPSCoordinate &, uint32_t proximity_threshold = 0 );

	bool isStory( void ){ return(!this->videos.empty()); }

		/* Specify which is the current video
		 * <- false if not found
		 */
	bool currentVideo(const char *);
	StoryVideo &getCurrentStoryVideo( void ){
		return this->videos[ this->current_video_idx ];
	}

	enum pkind {
		AFTERTRACE=0,		// A trace outside any video and after the current video
		BEFORETRACE,	// A trace outside any video and before the current video
		CURRENTVIDEO,	// Inside the current video
		AFTERVIDEO,		// Inside a video after the current one
		BEFOREVIDEO		// Inside a video before the current one
	};

	enum pkind positionKind(int idx);
};

#endif

