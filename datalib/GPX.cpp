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

	printf("\tTime : ");
	printtm(this->getMin().getGMT());
	printf(" -> ");
	printtm(this->getMax().getGMT());
	puts("");

	if(debug){
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

GPX::GPX( const char *file ){
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

			/* Update min/max */
		if(!this->getSampleCount()){
			this->getMin().set( lat, lgt, alt, time );
			this->getMax().set( lat, lgt, alt, time );
		} else {
			if(lat < this->getMin().getLatitude())
				this->getMin().setLatitude(lat);
			if(lat > this->getMax().getLatitude())
				this->getMax().setLatitude(lat);

			if(lgt < this->getMin().getLongitude())
				this->getMin().setLongitude(lgt);
			if(lgt > this->getMax().getLongitude())
				this->getMax().setLongitude(lgt);

			if(alt < this->getMin().getAltitude())
				this->getMin().setAltitude(alt);
			if(alt > this->getMax().getAltitude())
				this->getMax().setAltitude(alt);

			if(this->getMin().getSampleTime() > time)
				this->getMin().setSampleTime(time);
			if(this->getMax().getSampleTime() < time)
				this->getMax().setSampleTime(time);
		}

			/* store the new sample */
		GpxData nv(lat, lgt, alt, time);
		if(!this->samples.empty())
			nv.addDistance( this->getLast() );
	
			/* insert the new sample in the list */
		this->samples.push_back(nv);
	}

	fclose(f);
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
