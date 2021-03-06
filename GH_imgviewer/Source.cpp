#include <Windows.h>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <AssimpCV.h>
#include <recons_common.h>
#include <gh_common.h>
#include <cv_draw_common.h>

int main(int argc, char ** argv){
	if (argc < 2){
		std::cout << "pls enter the directory";
		return 0;
	}

	std::string out_directory;
	bool write_im = argc >= 3;
	if (write_im){
		out_directory = std::string(argv[2]);
		CreateDirectory(out_directory.c_str(), NULL);
	}

	std::string directory(argv[1]);

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

	std::vector<std::string> filenames;
	std::vector<FrameData> frame_data;
	std::vector<FrameDataProcessed> frame_data_processed;
	std::vector<PointMap> pointmaps;

	int current_frame=0;

	while (true){
		filenames.clear();
		frame_data.clear();
		frame_data_processed.clear();
		pointmaps.clear();

		std::cout << "current frame: " << current_frame << std::endl;

		filenameSS.str("");
		filenameSS << directory << "/" << current_frame << ".xml.gz";

		filenames.push_back(filenameSS.str());

		load_frames(filenames, pointmaps, frame_data);
		//load_processed_frames(filenames, bpdv.size(), frame_data_processed);

		if (!frame_data.empty())
		{
			cv::imshow("full picture", frame_data[0].mmColor);
			cv::Mat depth_vis = visualize_float(frame_data[0].mmDepth);
			cv::imshow("depth", depth_vis);

			if (write_im){
				std::stringstream ss;
				ss << out_directory << "/rgb" << current_frame << ".png";
				cv::imwrite(ss.str(), frame_data[0].mmColor);
				ss.str("");
				ss << out_directory << "/depth" << current_frame << ".png";
				cv::imwrite(ss.str(), depth_vis);
			}
		}

		//if (!frame_data_processed.empty()){
		//	std::stringstream winSS;
		//	for (int i = 0; i < bpdv.size(); ++i){
		//
		//		if (frame_data_processed[0].mBodyPartImages[i].mMat.empty()) continue;
		//
		//		winSS.str("");
		//		winSS << " body part " << i;
		//		cv::imshow(winSS.str(), frame_data_processed[0].mBodyPartImages[i].mMat);
		//	}
		//}

		cv::waitKey(25);
		++current_frame;

		//char inp = cv::waitKey();
		//
		//if (inp == 'z'){
		//	--current_frame;
		//}
		//else if (inp == 'x'){
		//	++current_frame;
		//}
	}
}