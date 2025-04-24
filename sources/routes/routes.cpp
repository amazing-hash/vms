#include "routes.h"

static std::string fileReader(const std::string & filename) {
    std::stringstream buffer;
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        while (file.good()) {
            char buf[4096];
            memset(buf, '\0', sizeof (buf));
            file.read((char*) &buf, sizeof (buf) - 1);
            buffer << buf;
        }
        file.close();
        return buffer.str();
    }
    throw vms::session::exception::SessionException("File not found " + filename);
}

vms::IHandlerResponse* fetch_index_page(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session) {
    try {
        kainjow::mustache::mustache tmpl{fileReader("static/index.html")};
        kainjow::mustache::data cameras_data{kainjow::mustache::data::type::list};
        json json_cameras = vms::CameraStorage::getInstance()->getJSON();
        for (auto & json_camera : json_cameras["cameras"]) {
            for (auto & json_channel : json_camera["channels"]) {
                kainjow::mustache::data camera_data;
                int id = json_camera["id"].get<int>();
                std::string descrtiption = json_camera["description"].get<std::string>();
                std::string login = json_camera["login"].get<std::string>();
                if (login.empty())
                    login = "not used";
                std::string password = json_camera["password"].get<std::string>();
                if (password.empty())
                    password = "not used";
                int port = json_camera["port"].get<int>();
                std::string state = json_channel["state"].get<std::string>();
                int channel = json_channel["channel"].get<int>();
                std::string url = json_channel["url"].get<std::string>();
                std::string i_codec = json_channel["i-codec"].get<std::string>();
                std::string i_type = json_channel["i-type"].get<std::string>();
                std::string o_codec = json_channel["o-codec"].get<std::string>();
                std::string o_type = json_channel["o-type"].get<std::string>();
                camera_data.set("id", std::to_string(id));
                camera_data.set("description", descrtiption);
                camera_data.set("login", login);
                camera_data.set("password", password);
                camera_data.set("port", std::to_string(port));
                camera_data.set("state", state);
                camera_data.set("channel", std::to_string(channel));
                camera_data.set("url", url);
                camera_data.set("i-codec", i_codec);
                camera_data.set("i-type", i_type);
                camera_data.set("o-codec", o_codec);
                camera_data.set("o-type", o_type);
                if (state == "on")
                    camera_data.set("button-type", "btn btn-success");
                else
                    camera_data.set("button-type", "btn btn-secondary");

                cameras_data.push_back(camera_data);
            }
        }
        return new vms::HandlerFetchTemplateHtml(tmpl.render({"cameras", cameras_data}));
    } catch (vms::session::exception::SessionException & ex) {
        LOGE << ex.what();
        throw vms::session::exception::SessionException("Processing error in the dispatcher. No matches found url " + url);
    } catch (...) {
        LOGE << "Invalid format json data";
        exit(EXIT_FAILURE);
    }
}

vms::IHandlerResponse* fetch_statistics_page(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session) {
    try {
        kainjow::mustache::mustache tmpl{fileReader("static/statistics.html")};
        kainjow::mustache::data cameras_data{kainjow::mustache::data::type::list};
        json json_cameras = vms::CameraStorage::getInstance()->getJSON();
        for (auto & json_camera : json_cameras["cameras"]) {
            for (auto & json_channel : json_camera["channels"]) {
                kainjow::mustache::data camera_data;
                int id = json_camera["id"].get<int>();
                std::string descrtiption = json_camera["description"].get<std::string>();
                int channel = json_channel["channel"].get<int>();
                std::string url = json_channel["url"].get<std::string>();
                std::string i_codec = json_channel["i-codec"].get<std::string>();
                std::string i_type = json_channel["i-type"].get<std::string>();
                std::string o_codec = json_channel["o-codec"].get<std::string>();
                std::string o_type = json_channel["o-type"].get<std::string>();
                camera_data.set("id", std::to_string(id));
                camera_data.set("description", descrtiption);
                camera_data.set("channel", std::to_string(channel));
                camera_data.set("url", url);
                camera_data.set("i-codec", i_codec);
                camera_data.set("i-type", i_type);
                camera_data.set("o-codec", o_codec);
                camera_data.set("o-type", o_type);

                vms::Statistics * statistics = vms::Statistics::getInstance();
                camera_data.set("connection_online", std::to_string(statistics->m_connections[id * 100 + channel]));
                cameras_data.push_back(camera_data);
            }
        }
        return new vms::HandlerFetchTemplateHtml(tmpl.render({"cameras", cameras_data}));
    } catch (vms::session::exception::SessionException & ex) {
        LOGE << ex.what();
        throw vms::session::exception::SessionException("Processing error in the dispatcher. No matches found url " + url);
    } catch (...) {
        LOGE << "Invalid format json data";
        exit(EXIT_FAILURE);
    }
}

vms::IHandlerResponse* fetch_camera_view_page(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session) {
    try {
        kainjow::mustache::mustache tmpl{fileReader("static/camera-view.html")};
        kainjow::mustache::data camera_data;
        camera_data.set("id", params["camera"]);
        camera_data.set("channel", params["channel"]);
        camera_data.set("description", params["description"]);
        camera_data.set("url", params["url"]);
        return new vms::HandlerFetchTemplateHtml(tmpl.render({"camera", camera_data}));
    } catch (vms::session::exception::SessionException & ex) {
        LOGE << ex.what();
        throw vms::session::exception::SessionException("Processing error in the dispatcher. No matches found url " + url);
    } catch (...) {
        LOGE << "Invalid format json data";
        exit(EXIT_FAILURE);
    }
}

vms::IHandlerResponse* fetch_camera_stream(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session) {
    try {
        if (tokens.size() == 6) {
            int camera = atoi(tokens[4].c_str());
            int channel = atoi(tokens[5].c_str());
            vms::Statistics * statistics = vms::Statistics::getInstance();
            statistics->m_connections[camera * 100 + channel]++;
            statistics->m_relation_connection_and_camera[session->m_socket] = camera * 100 + channel;
            return new vms::HandlerViewVideoStream(camera * 100 + channel);
        }
    } catch (vms::session::exception::SessionException & ex) {
        LOGE << ex.what();
        throw vms::session::exception::SessionException("Processing error in the dispatcher. No matches found url " + url);
    }
}

vms::IHandlerResponse* fetch_camera_configuration_page(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session) {
    try {
        kainjow::mustache::mustache tmpl{fileReader("static/configuration.html")};
        kainjow::mustache::data configuration_data;
        json json_cameras = vms::CameraStorage::getInstance()->getJSON();
        configuration_data.set("cameras", json_cameras.dump());
        return new vms::HandlerFetchTemplateHtml(tmpl.render(configuration_data));
    } catch (vms::session::exception::SessionException & ex) {
        LOGE << ex.what();
        throw vms::session::exception::SessionException("Processing error in the dispatcher. No matches found url " + url);
    } catch (...) {
        LOGE << "Invalid format json data";
        exit(EXIT_FAILURE);
    }
}

vms::IHandlerResponse* fetch_css_resources(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session) {
    if (tokens.size() == 2)
        return new vms::HandlerFetchCss("static/css/" + tokens[1]);
}

vms::IHandlerResponse* fetch_js_resources(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session) {
    if (tokens.size() == 2)
        return new vms::HandlerFetchJs("static/js/" + tokens[1]);
}

vms::IHandlerResponse* fetch_svg_resources(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session) {
    if (tokens.size() == 2)
        return new vms::HandlerFetchSvg("static/svg/" + tokens[1]);
}

vms::IHandlerResponse* save_camera_settings(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session) {
    json response;
    //    std::map<std::string, std::string> forms_params = vms::RequestFormParse::parse_form_params(body);
    std::string _body = body;
    vms::RequestFormParse::trim(_body);
    response["status"] = "error";
    std::lock_guard<std::mutex> lock(mutex_configuration_file);
    std::ofstream out;
    out.open("db/cameras.json");
    if (out.is_open())
        out << _body;
    out.close();
    response["status"] = "success";
    return new vms::HandlerFetchJson(response);
}