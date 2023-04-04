#include <sstream>
const std::string input_file = "../../data/out.obj";

int main() {
    std::ifstream input_stream;
    input_stream.open(input_file);
    std::map<int, vec3> vertices;
    std::vector<std::vector<int>> faces;
    if (input_stream.is_open()) {
        std::string line;
        while (std::getline(input_stream, line)){
            std::istringstream iss(line);
            char type;
            iss >> type;
            int i = 1;

            if (type == 'v') {
                double x, y, z;
                iss >> x >> y >> z;
                vertices.insert(std::make_pair(i, vec3(x,y,z)));
            } else if (type == 'f') {
                std::vector<int> indices;
                int index;
                while (iss >> index) {
                    indices.push_back(index);
                }
                faces.push_back(indices);
            }
        }
    }


    std::vector<std::shared_ptr<hittable>> objects;
    for (const auto& face: faces) {
        Point3 p0 = vertices.find(face[0])->second;
        Point3 p1 = vertices.find(face[1])->second;
        Point3 p2 = vertices.find(face[2])->second;
        triangle tri = triangle(p0, p1, p2);
        objects.push_back(std::make_shared<triangle>(tri));
    }

}
