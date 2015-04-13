#include <iostream>
#include <opencv2\opencv.hpp>
#include <AssimpCV.h>
#include <recons_common.h>
#include <gh_common.h>
#include <gh_search.h>
#include <cv_draw_common.h>

int main(int argc, char ** argv){
	if (argc < 4){
		std::cout << "pls enter the directory, start frame, and numframes";
		return 0;
	}

	std::string directory(argv[1]);

	unsigned int startframe, numframes;

	startframe = atoi(argv[2]);
	numframes = atoi(argv[3]);

	BodyPartDefinitionVector bpdv;

	std::stringstream filenameSS;
	cv::FileStorage fs;

	filenameSS << directory << "/bodypartdefinitions.xml.gz";

	fs.open(filenameSS.str(), cv::FileStorage::READ);
	for (auto it = fs["bodypartdefinitions"].begin();
		it != fs["bodypartdefinitions"].end();
		++it){
		BodyPartDefinition bpd;
		read(*it, bpd);
		bpdv.push_back(bpd);
	}
	fs.release();

	BodypartFrameCluster bodypart_frame_clusters;

	filenameSS.str("");
	filenameSS << directory << "/clusters-" << "startframe" << startframe << "numframes" << numframes << ".xml.gz";

	fs.open(filenameSS.str(), cv::FileStorage::READ);
	read(fs["bodypart_frame_clusters"], bodypart_frame_clusters);
	fs.release();
	filenameSS.str("");

	std::vector<std::string> filenames;
	std::vector<FrameData> frame_data;
	std::vector<FrameDataProcessed> frame_data_processed;
	std::vector<PointMap> pointmaps;

	int current_bodypart = 0;
	int current_cluster = 0;
	int current_frame_within_cluster = 0;

	cv::namedWindow("body part", CV_WINDOW_NORMAL);

	while (true){
		filenames.clear();
		frame_data.clear();
		frame_data_processed.clear();
		pointmaps.clear();

		if (current_bodypart < 0){
			current_bodypart = bpdv.size() - 1;
			current_cluster = 0;
			current_frame_within_cluster = 0;
		}

		if (current_bodypart >= bpdv.size()){
			current_bodypart = 0;
			current_cluster = 0;
			current_frame_within_cluster = 0;
		}

		if (current_cluster < 0){
			current_cluster = bodypart_frame_clusters[current_bodypart].size() - 1;
			current_frame_within_cluster = 0;
		}

		if (current_cluster >= bodypart_frame_clusters[current_bodypart].size()){
			current_cluster = 0;
			current_frame_within_cluster = 0;
		}

		if (current_frame_within_cluster < 0){
			current_frame_within_cluster = bodypart_frame_clusters[current_bodypart][current_cluster].size() - 1;
		}

		if (current_frame_within_cluster >= bodypart_frame_clusters[current_bodypart][current_cluster].size()){
			current_frame_within_cluster = 0;
		}

		int current_frame = bodypart_frame_clusters[current_bodypart][current_cluster][current_frame_within_cluster];

		std::cout << "current bodypart: " << current_bodypart << std::endl
			<< "current cluster: " << current_cluster << std::endl
			<< "current frame: " << current_frame << std::endl
			<< "current frame within cluster: " << current_frame_within_cluster << std::endl
			<< "total frames within cluster: " << bodypart_frame_clusters[current_bodypart][current_cluster].size() << std::endl;

		filenameSS.str("");
		filenameSS << directory << "/" << current_frame << ".xml.gz";

		filenames.push_back(filenameSS.str());

		load_frames(filenames, pointmaps, frame_data);
		load_processed_frames(filenames, bpdv.size(), frame_data_processed);

		std::cout << "---------------------------\n";


		if (!frame_data_processed.empty()){

			cv::imshow("body part", frame_data_processed[0].mBodyPartImages[current_bodypart].mMat);
		}

		char inp = cv::waitKey();

		if (inp == 'q'){
			--current_bodypart;
		}
		else if (inp == 'w'){
			++current_bodypart;
		}

		if (inp == 'a'){
			--current_cluster;
		}
		else if (inp == 's'){
			++current_cluster;
		}

		if (inp == 'z'){
			--current_frame_within_cluster;
		}
		else if (inp == 'x'){
			++current_frame_within_cluster;
		}
	}
}