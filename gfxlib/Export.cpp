/* Export.h
 * Export GPMF as standard GPS files
 */

#include "Export.h"

#include <cstdio>

void Export::generateGPX( const char *fulltarget, char *filename, char *iname ){
	sprintf(filename, "%s.gpx", iname);

	FILE *f = fopen(fulltarget,"w");
	if(!f){
		perror(fulltarget);
		return;
	}

	fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		"<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:gpxtpx=\"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" version=\"1.1\" creator=\"GPMFMetersGenerator\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd http://www.garmin.com/xmlschemas/TrackPointExtension/v1 http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd\">\n", f);
	fprintf(f, "\t<metadata>\n"
	    "\t\t<name>%s</name>\n"
  		"\t</metadata>\n", iname
	);
	fprintf(f, "\t<trk>\n"
	    "\t\t<name>%s</name>\n"
		"\t\t<trkseg>\n", iname
	);
	
	for(struct GPMFdata *p = this->video.getFirst(); p; p = p->next){
		fprintf(f, "\t\t\t<trkpt lat=\"%f\" lon=\"%f\">\n", p->latitude, p->longitude);
		fprintf(f, "\t\t\t\t<ele>%f</ele>\n", p->altitude);
		fputs("\t\t\t</trkpt>\n", f);
	}

	fputs( "\t\t</trkseg>\n"
		"\t</trk>\n"
		"</gpx>\n", f);

	fclose(f);

	if(verbose)
		printf("'%s' generated\n", fulltarget);

}
