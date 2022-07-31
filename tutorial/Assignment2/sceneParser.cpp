#include <string>
#include "./sceneParser.h"

Eigen::Vector4f SceneParser::parseVec4(const std::string& line)
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();
    
    unsigned int vertIndexStart = 2;
    
    while(vertIndexStart < tokenLength)
    {
        if(tokenString[vertIndexStart] != ' ')
            break;
        vertIndexStart++;
    }
    
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float x = ParseFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float y = ParseFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float z = ParseFloatValue(line, vertIndexStart, vertIndexEnd);
    
	vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float w = ParseFloatValue(line, vertIndexStart, vertIndexEnd);

    return Eigen::Vector4f(x,y,z,w);

    // glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()))
}

SceneParser::SceneParser(const std::string& fileName, SceneData* data)
{
	std::ifstream file;
    file.open((fileName).c_str());
	std::vector<Eigen::Vector4f> notRefobjects,notRefColor,refobjects,refColor;
	std::vector<int> refIndx,notRefIndx,trIndx;
    std::string line;
	int counter = 0, rCounter = 0, tCounter = 0;
    if(file.is_open())
    {
        while(file.good())
        {
            getline(file, line);
        
            unsigned int lineLength = line.length();
            
            if(lineLength < 2)
                continue;
            
            const char* lineCStr = line.c_str();
            
            switch(lineCStr[0])
            {
			
                case 'e'://The camera coords. w is additional
					data->eye = parseVec4(line);			
				break;
				case 'a'://ambient light
					data->ambient = parseVec4(line);
				break;
				case 'r'://reflective object
					refobjects.push_back(parseVec4(line));
					refIndx.push_back(counter);
					counter++;
					rCounter++;
					break;
				case 't'://transparent object
					data->objects.push_back(parseVec4(line));
					trIndx.push_back(counter);
					counter++;
					tCounter++;
					break;
				case 'o'://not reflect or transparent object (regular)
					notRefIndx.push_back(counter);
					counter++;
					notRefobjects.push_back( parseVec4(line));
				break;
				case 'c'://ambient&diffuse colors of objects
					notRefColor.push_back( parseVec4(line));
				break;
				case 'd'://light direction: (x,y,z). [w=0]:directionalLight,[w=1]:spotLight
					data->directions.push_back( parseVec4(line));
				break;
				case 'p'://spotLight's position: (x,y,z). w- the cosine of the angle
					data->lights.push_back( parseVec4(line));
				break;
				case 'i'://light's intensity
					data->intensities.push_back( parseVec4(line));
				break;
				
			}
		}
		for (int i = 0; i < data->objects.size(); i++)
		{
			data->colors.push_back(notRefColor[trIndx[i]]);
		}
		for (int i = 0; i < refobjects.size(); i++)
		{
			data->objects.push_back(refobjects[i]);
			data->colors.push_back(notRefColor[refIndx[i]]);
		}
		for (int i = 0; i < notRefobjects.size(); i++)
		{
			data->objects.push_back(notRefobjects[i]);
			data->colors.push_back(notRefColor[notRefIndx[i]]);
		}
		data->sizes =  Eigen::Vector4i(data->objects.size(),data->directions.size(),rCounter,tCounter);
	}
	else
	{
		char buf[100];
		std::cout<<"can not open file!"<<std::endl;
		strerror_s(buf,errno);
		std::cerr << "Error: " << buf; 
		data->sizes = Eigen::Vector4i(0,0,0,0);
	}
    file.close();
	
}
