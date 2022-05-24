/* GPX
 * 	Impersonates GPX data
 *
 * 	DEV-NOTE : a clean way should have been to use libxml++ (or at least 
 * 	libxml2) in SAX mode.
 * 	Unfortunately, I didn't found a way to use-it in a portable way without
 * 	using autoconf. Consequently, I decided for the moment to keep my
 * 	quick and (very) dirty v1.0 XML parser which is enough to parse
 * 	CORRECTLY FORMATED GPX files.
 */

#include "../datalib/Context.h"
#include "GPX.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cassert>

#define BUFFLEN	1024

char buff[BUFFLEN];
char *pbuff;	/* Pointer inside the buffer */

	/* clear the buffer */
static void clear(){
	pbuff = buff;
	*pbuff = 0;
}

	/* skip all char up incomming char (mostly for '<' and '>')
	 *
	 * <- false in case of error or end of file
	 */
static bool skipTocChar(FILE *f, char c){
	while((*pbuff = fgetc(f)) != c){
		if(feof(f))
			return false;
	}

	pbuff++;
	return true;
}

	/* read and store characters up to c
	 *
	 * <- false : end of file reached
	 */
static bool readUptoChar(FILE *f, char c){
	while((*(pbuff++) = fgetc(f)) !=c){
		if(feof(f))
			return false;
		if(pbuff - buff >= BUFFLEN){
			puts("*F* string too long");
			exit(EXIT_FAILURE);
		}
	}

	*pbuff = 0;
	return true;
}

	/* Look for "string"
	 *
	 * <- false : end of file
	 */
static bool lookFor(FILE *f, const char *string){
	while(true){
		const char *s = string;

		clear();

		if(!skipTocChar(f,*string))	/* Looks for the 1st char */
			return false;

		for(s = string+1; *s; s++)	/* reads remaining */
			if((*(pbuff++) = fgetc(f)) != *s){
				if(feof(f))
					return false;
				else
					break;
			}

		if(!*s)	/* string found */
			break;
	}

	*pbuff = 0;
	return true;
}

void GPX::Dump( void ){
	puts("*I* GPX min/max:");
	printf("\tlatitude : %f -> %f (%f)\n", this->getMin().getLatitude(), this->getMax().getLatitude(), this->getMax().getLatitude() - this->getMin().getLatitude());
	printf("\tlongitude : %f -> %f (%f)\n", this->getMin().getLongitude(), this->getMax().getLongitude(), this->getMax().getLongitude() - this->getMin().getLongitude());
	printf("\taltitude: %f -> %f (%f)\n", this->getMin().getAltitude(), this->getMax().getAltitude(), this->getMax().getAltitude() - this->getMin().getAltitude());
	printf("\tDistance covered : %f m\n", this->getLast().getCumulativeDistance() );

	printf("\tTime : ");
	printtm(this->getMin().getGMT());
	printf(" -> ");
	printtm(this->getMax().getGMT());
	puts("");

	if(this->isStory())
		printf("\tIt's a story with %lu videos loaded\n", this->videos.size());

	if(debug){
		if(this->isStory()){
			puts("\tStory's videos :");
			for( auto v: this->videos )
				printf("\t\t%s : %05d -> %05d\n", v.c_str(), v.start, v.end);
		}

		puts("*D* Memorized CPX data");
		for(auto p : samples){
			printf("\tLatitude : %.3f deg\n", p.getLatitude());
			printf("\tLongitude : %.3f deg\n", p.getLongitude());
			printf("\tAltitude : %.3f m\n", p.getAltitude());
			printf("\tCom distance : %.0f m\n", p.getCumulativeDistance());

			printf("\tTime : ");
			printtm(p.getGMT());
			puts("");
		}
	}

	printf("*I* %u memorised GPX\n", this->getSampleCount());
}

void GPX::updMinMax( GpxData &nv ){
	if(!this->getSampleCount()){
		this->getMin().set( static_cast<GPSCoordinate &>(nv) );
		this->getMax().set( static_cast<GPSCoordinate &>(nv) );
	} else {
		if(nv.getLatitude() < this->getMin().getLatitude())
			this->getMin().setLatitude(nv.getLatitude());
		if(nv.getLatitude() > this->getMax().getLatitude())
			this->getMax().setLatitude(nv.getLatitude());

		if(nv.getLongitude() < this->getMin().getLongitude())
			this->getMin().setLongitude(nv.getLongitude());
		if(nv.getLongitude() > this->getMax().getLongitude())
			this->getMax().setLongitude(nv.getLongitude());

		if(nv.getAltitude() < this->getMin().getAltitude())
			this->getMin().setAltitude(nv.getAltitude());
		if(nv.getAltitude() > this->getMax().getAltitude())
			this->getMax().setAltitude(nv.getAltitude());

		if(this->getMin().getSampleTime() > nv.getSampleTime())
			this->getMin().setSampleTime(nv.getSampleTime());
		if(this->getMax().getSampleTime() < nv.getSampleTime())
			this->getMax().setSampleTime(nv.getSampleTime());
	}
}

void GPX::readGPX( const char *file ){
	FILE *f;

	if(!(f = fopen(file, "r"))){
		perror(file);
		exit(EXIT_FAILURE);
	}

	while(true){
		double lat=NAN, lgt=NAN, alt=NAN;

			/* Read position */

		if(!lookFor(f,"<trkpt "))	// End of file reached
			break;

		if(!readUptoChar(f, '>'))	// read the full tag
			break;

		if(sscanf(buff, "<trkpt lat=\"%lf\" lon=\"%lf\">", &lat, &lgt) != 2)
			puts("*E* can't read values");

			/* Read elevation */
		if(!lookFor(f,"<ele>")){
			puts("*F* No elevation");
			exit(EXIT_FAILURE);
		}

		if(!readUptoChar(f, '<')){	/* read up to closing tag */
			puts("*F* malformed elevation");
			exit(EXIT_FAILURE);
		}

		if(sscanf(buff, "<ele>%lf<", &alt) != 1)
			puts("*E* can't read altitude values");


			/* Read timestamp */
		if(!lookFor(f,"<time>")){
			puts("*F* No time");
			exit(EXIT_FAILURE);
		}
	
		if(!readUptoChar(f, '<')){	/* read up to closing tag */
			puts("*F* malformed time");
			exit(EXIT_FAILURE);
		}

		struct tm t;
		memset(&t, 0, sizeof(struct tm));
		if(!strptime(buff, "<time>%FT%T%z", &t)){
			puts("*E* can't read time");
			exit(EXIT_FAILURE);
		}

		long int sample_offset = t.tm_gmtoff;
		if(debug){
			printf(" ");
			printtm(&t);
			puts("");
		}

			/* mktime() considers only in account the current TZ
			 * Consequently, it's needed to adjust the timestamps
			 * as per the initial offset AS WELL AS the current one.
			 * CAUTION : timezone is the offset of UTC compared to the 
			 * current TZ. Consequently, it is the REVERSE of __tm_gmtoff
			 */
		time_t time = mktime( &t );
		time -= timezone + sample_offset;

#if 0	/* Sanity check */
		if(debug){
			struct tm *tm = gmtime(&time);
			printf(">");
			printtm(tm);
			puts("");
		}
#endif


			/* store the new sample */
		GpxData nv(lat, lgt, alt, time);
		updMinMax(nv);
		if(!this->samples.empty())
			nv.addDistance( this->getLast() );
	
			/* insert the new sample in the list */
		this->samples.push_back(nv);
	}

	fclose(f);
}

void GPX::readStory( const char *file ){
	FILE *f;

	if(!(f = fopen(file, "r"))){
		perror(file);
		exit(EXIT_FAILURE);
	}
	
	if(!fgets(buff, sizeof(buff), f)){
		fputs("*F* Can't read Story's magic\n", stderr);
		exit(EXIT_FAILURE);
	}
	buff[strcspn(buff,"\n")] = 0;
	if(strcmp(buff, "GPMFStory 1.0")){
		fputs("*F* Story's magic not found\n", stderr);
		exit(EXIT_FAILURE);
	}

		/* Reading videos anchors
		 *	Rude checks are done here (assert()) as we are relying on
		 *	formating made by mkStory.
		 */
	while(fgets(buff, sizeof(buff), f)){
		if(*buff == '#')
			continue;
		if(*buff == '*')
			break;

		char *indexes = strchr(buff, ',');
		assert(indexes);

		*indexes++ = 0;

		int istart, iend;
		assert( sscanf(indexes, "%d, %d", &istart, &iend) == 2 );

		StoryVideo nv(buff, istart, iend);
		this->videos.push_back(nv);
	}

		/* Reading GPX data */
	while(fgets(buff, sizeof(buff), f)){
		double lat, lgt, alt;
		time_t st;
		double dst;

		if(*buff == '#')
			continue;

		if(sscanf(buff, "%lf, %lf, %lf, %lu, %lf", &lat, &lgt, &alt, &st, &dst) != 5){
			fputs("*F* Failed to read GPX contents\n", stderr);
			exit(EXIT_FAILURE);
		}
		GpxData nv(lat, lgt, alt, st, dst);
		updMinMax(nv);
		this->samples.push_back(nv);
	}
	
	fclose(f);
}

GPX::GPX( const char *file, bool story ){
	if(story)
		readStory( file );
	else
		readGPX( file );
}

bool GPX::sameArea( GPSCoordinate &coord, uint32_t proximity_threshold){
	if( this->getMin().getLatitude() - proximity_threshold > coord.getLatitude() ||
		this->getMax().getLatitude() + proximity_threshold < coord.getLatitude() )
		return false;

	if( this->getMin().getLongitude() - proximity_threshold > coord.getLongitude() ||
		this->getMax().getLongitude() + proximity_threshold < coord.getLongitude() )
		return false;
	
	return true;
}
