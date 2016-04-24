//
//  EditorComponents.inl
//  EnginePrototype
//
//  Created by Samir Sinha on 4/21/16.
//
//

namespace cinek {

template<typename Object>
void updateComponentUI
(
    EditorUIVariantMap& uiVariants,
    float columnOffsets[2],
    const char* componentName,
    Object* object
)
{
    ove::Component<Object> component(object);
    
    if (ImGui::CollapsingHeader(componentName, nullptr, true, true)) {
        ImGui::Columns(2);
        ImGui::SetColumnOffset(0, columnOffsets[0]);
        ImGui::SetColumnOffset(1, columnOffsets[1]);
        
        //  label on the left, editable on the right.
        component.enumerateProperties(
            [componentName](const char* label, ove::ComponentData& data) -> bool
            {
                char widgetId[32];
                float floats[4];
                bool writeProperty = false;
                snprintf(widgetId, sizeof(widgetId), "##%s_%s", componentName, label);
                widgetId[sizeof(widgetId)-1] = '\0';
                
                ImGui::Selectable(label);
                ImGui::NextColumn();
                ImGui::PushItemWidth(-1);
                
                switch(data.type()) {
                case ove::ComponentData::Type::kPoint3: {
                    auto pos = data.get<ckm::vector3>();
                    floats[0] = pos[0];
                    floats[1] = pos[1];
                    floats[2] = pos[2];
                    writeProperty = ImGui::InputFloat3(widgetId, floats, 4);
                    if (writeProperty) {
                        pos[0] = floats[0];
                        pos[1] = floats[1];
                        pos[2] = floats[2];
                        data.set(pos);
                    }
                }
                break;
                case ove::ComponentData::Type::kQuaternion: {
                    auto quat = data.get<ckm::quat>();
                   
                    char text[48];
                    snprintf(text, sizeof(text), "(%1.2f, %1.2f, %1.2f, %1.2f)",
                        quat[0], quat[1], quat[2], quat[3]);
                   
                    if (ImGui::Selectable(text)) {
                        snprintf(widgetId, sizeof(widgetId), "##%s_%s", componentName, label);
                        ImGui::OpenPopup(widgetId);
                    }
    
                    if (ImGui::BeginPopup(widgetId)) {
                        ImGui::EndPopup();
                    }
                    /*
                    if (writeProperty) {
                        quat[0] = floats[0];
                        quat[1] = floats[1];
                        quat[2] = floats[2];
                        quat[3] = floats[3];
                        ckm::normalize(quat, quat);
                        data.set(quat);
                    }
                    */
                }
                break;
                default:
                break;
                }
                ImGui::PopItemWidth();
                ImGui::NextColumn();
                return writeProperty;
            }
        );
        
        ImGui::Columns(1);
    }
    
}

}