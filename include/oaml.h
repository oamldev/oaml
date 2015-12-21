#ifndef __OAML_H__
#define __OAML_H__

int oamlInit(const char *pathToMusic);
void oamlSetAudioFormat(int freq, int channels, int bytes);
int oamlPlayTrack(const char *name);
int oamlPlayTrackId(int id);
void oamlSetCondition(int id, int value);
void oamlMixToBuffer(void *buffer, int size);
void oamlShutdown();

#endif /* __OAML_H__ */
