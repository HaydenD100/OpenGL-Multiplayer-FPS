#include "Texture3D.h"


void Texture3D::Create(int width, int height, int depth) {
	this->width = width;
	this->height = height;
	this->depth = depth;
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_3D, m_ID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

}
void Texture3D::Clear() {
	glBindTexture(GL_TEXTURE_3D, m_ID);
	uint32_t z = 0;
	glClearTexImage(m_ID, 0, GL_RGBA16F, GL_UNSIGNED_BYTE, &z);
}

void Texture3D::Load(const std::string path) {

} 
unsigned int Texture3D::GetId() {
	return m_ID;
}
void Texture3D::Bind(unsigned int slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_3D, m_ID);
}
void Texture3D::ImageBind(unsigned int slot) {
	glBindImageTexture(slot, m_ID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
}
void Texture3D::UnImageBind(unsigned int slot) {
	glBindImageTexture(slot, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
}

int Texture3D::GetWidth() {
	return width;
}
int Texture3D::GetHeight() {
	return height;
}
int Texture3D::GetDepth() {
	return depth;
}