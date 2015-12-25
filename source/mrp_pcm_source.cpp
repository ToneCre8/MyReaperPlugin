#include "mrp_pcm_source.h"

PCM_source * MRP_PCMSource::Duplicate()
{
	return nullptr;
}

bool MRP_PCMSource::IsAvailable()
{
	return m_dsp!=nullptr;
}

const char * MRP_PCMSource::GetType()
{
	return "MRP_PCMSOURCE";
}

bool MRP_PCMSource::SetFileName(const char * newfn)
{
	return false;
}

int MRP_PCMSource::GetNumChannels()
{
	return 2;
}

double MRP_PCMSource::GetSampleRate()
{
	return 44100.0;
}

double MRP_PCMSource::GetLength()
{
	return 1000000.0;
}

int MRP_PCMSource::PropertiesWindow(HWND hwndParent)
{
	return 0;
}

void MRP_PCMSource::GetSamples(PCM_source_transfer_t * block)
{
	if (m_dsp->is_prepared() == false)
		m_dsp->prepare_audio(block->nch, block->samplerate, block->length);
	if (m_dsp->is_prepared() == true)
	{
		m_dsp->process_audio(block->samples, block->length);
	}
	else
	{
		// output silence if the dsp couldn't prepare itself
		for (int i = 0; i < block->length*block->nch; ++i)
			block->samples[i] = 0.0;
	}
	block->samples_out = block->length;
}

void MRP_PCMSource::GetPeakInfo(PCM_source_peaktransfer_t * block)
{
}

void MRP_PCMSource::SaveState(ProjectStateContext * ctx)
{
}

int MRP_PCMSource::LoadState(const char * firstline, ProjectStateContext * ctx)
{
	return 0;
}

void MRP_PCMSource::Peaks_Clear(bool deleteFile)
{
}

int MRP_PCMSource::PeaksBuild_Begin()
{
	return 0;
}

int MRP_PCMSource::PeaksBuild_Run()
{
	return 0;
}

void MRP_PCMSource::PeaksBuild_Finish()
{
}

int MRP_PCMSource::Extended(int call, void *parm1, void *parm2, void *parm3)
{
	if (call == PCM_SOURCE_EXT_ENDPLAYNOTIFY)
	{
		m_dsp->release_audio();
		return 1;
	}
	return 0;
}

std::shared_ptr<MRP_PCMSource> g_test_source;
bool g_is_playing = false;
preview_register_t g_prev_reg = { 0 };

void test_pcm_source(int op)
{
	if (op == 0)
	{
		if (g_test_source == nullptr)
		{
			auto mydsp = std::make_shared<MyTestAudioDSP>();
			g_test_source = std::make_shared<MRP_PCMSource>(mydsp);
			g_prev_reg.src = g_test_source.get();
			g_prev_reg.volume = 1.0;
			InitializeCriticalSection(&g_prev_reg.cs);
		}
		if (g_is_playing == false)
		{
			PlayPreview(&g_prev_reg);
			g_is_playing = true;
		}
		else
		{
			StopPreview(&g_prev_reg);
			g_is_playing = false;
		}
	}
	else if (op == 1)
	{
		if (g_is_playing == true)
			StopPreview(&g_prev_reg);
		g_test_source.reset();
		DeleteCriticalSection(&g_prev_reg.cs);
	}
}
