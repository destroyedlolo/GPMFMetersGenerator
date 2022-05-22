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
	
	for(auto p : this->video.getSamples()){
		fprintf(f, "\t\t\t<trkpt lat=\"%f\" lon=\"%f\">\n", p.getLatitude(), p.getLongitude());
		fprintf(f, "\t\t\t\t<ele>%f</ele>\n", p.getAltitude());
		fputs("\t\t\t</trkpt>\n", f);
	}

	fputs( "\t\t</trkseg>\n"
		"\t</trk>\n"
		"</gpx>\n", f);

	fclose(f);

	if(verbose)
		printf("'%s' generated\n", fulltarget);
}

void Export::generateKML( const char *fulltarget, char *filename, char *iname ){
	sprintf(filename, "%s.kml", iname);

	FILE *f = fopen(fulltarget,"w");
	if(!f){
		perror(fulltarget);
		return;
	}


	fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
		"\t<Document>\n"
	    "\t\t<name>telemetry</name>\n"
    	"\t\t<description>Created by GPMFMetersGenerator</description>\n"
	, f);

		/* colors */
	fputs("\t\t<Style id=\"trace\">\n"
		"\t\t\t<LineStyle>\n"
        "\t\t\t\t<color>961400FF</color>\n"
        "\t\t\t\t<width>3</width>\n"
      	"\t\t\t</LineStyle>\n"
    	"\t\t</Style>\n"
	, f);

	fputs("\t\t<Style id=\"End\">\n"
		"\t\t\t<LineStyle>\n"
        "\t\t\t\t<color>FFEE58FF</color>\n"
        "\t\t\t\t<width>3</width>\n"
      	"\t\t\t</LineStyle>\n"
    	"\t\t</Style>\n"
	, f);

		/* Trace itself */
	fprintf(f, 
		"\t\t<Folder>\n"
		"\t\t\t<name>Trace</name>\n"
		"\t\t\t<open>1</open>\n"
		"\t\t\t<Placemark>\n"
		"\t\t\t\t<visibility>1</visibility>\n"
		"\t\t\t\t<styleUrl>#trace</styleUrl>\n"
		"\t\t\t\t<name>%s</name>\n"
		"\t\t\t\t<LineString>\n"
		"\t\t\t\t\t<tessellate>1</tessellate>\n"
		"\t\t\t\t\t<altitudeMode>clampToGround</altitudeMode>\n"
		"\t\t\t\t\t<coordinates>"
	, iname);

	for(auto p : this->video.getSamples()){
		fprintf(f, "%f,%f,%f\n", p.getLongitude(), p.getLatitude(), p.getAltitude());
	}

	fputs(
		"</coordinates>\n"
		"\t\t\t\t</LineString>\n"
		"\t\t\t</Placemark>\n"
	, f);

	fprintf(f,
		"\t\t\t<Placemark>\n"
/*		"\t\t\t<styleUrl>#depart</styleUrl>\n" */
		"\t\t\t\t<name>Starting point</name>\n"
		"\t\t\t\t<Point>\n"
		"\t\t\t\t\t<coordinates>%f,%f</coordinates>\n"
		"\t\t\t\t</Point>\n"
		"\t\t\t</Placemark>\n"
	, this->video.getFirst().getLongitude(), this->video.getFirst().getLatitude());

	fprintf(f,
		"\t\t\t<Placemark>\n"
		"\t\t\t<styleUrl>#End</styleUrl>\n"
		"\t\t\t\t<name>Finish</name>\n"
		"\t\t\t\t<Point>\n"
		"\t\t\t\t\t<coordinates>%f,%f</coordinates>\n"
		"\t\t\t\t</Point>\n"
		"\t\t\t</Placemark>\n"
	, this->video.getLast().getLongitude(), this->video.getLast().getLatitude());

	fputs(
		"\t\t</Folder>\n"
		"\t</Document>\n"
		"</kml>"
	, f);

	fclose(f);

	if(verbose)
		printf("'%s' generated\n", fulltarget);
}
