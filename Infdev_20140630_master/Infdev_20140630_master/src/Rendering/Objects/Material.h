#pragma once

#include <string>
#include <cppu/cgc/pointers.h>
#include <cppu/stor/vector.h>
#include <unordered_map>

#include "Rendering/rtypes.h"
#include "stdafx.h"
#include "Utils/Hash.h"

namespace Esteem
{
	class IShader;
	class AbstractBuffer;
	class Buffer;
	class Texture1D;
	class Texture2D;
	
	class Material
	{
	public:
		struct Buffers
		{
		private:
			struct pair_base
			{
				hash_t hash;
				cgc::strong_ptr<AbstractBuffer> buffer;

				pair_base() = default;
				pair_base(hash_t hash, const cgc::strong_ptr<AbstractBuffer>& buffer) : hash(hash), buffer(buffer) { }
				pair_base(const pair_base&) = default;
				pair_base(pair_base&&) = default;
			};

		public:
			template <class T>
			struct pair
			{
				const hash_t hash;
				const cgc::strong_ptr<T> buffer;
			};

			stor::vector<pair_base> data;
			uint8 bufferSize = 0;
			uint8 texture1DSize = 0;
			uint8 texture2DSize = 0;

			void SetBuffer(hash_t bindHash, const cgc::strong_ptr<Buffer>& buffer);
			void SetTexture1D(hash_t bindHash, const cgc::strong_ptr<Texture1D>& texture);
			void SetTexture2D(hash_t bindHash, const cgc::strong_ptr<Texture2D>& texture);
		};

	protected:
		std::string path;
		std::string name;

		bool transparent;
		bool reflective;
		bool doubleSided;

		Buffers buffers;

	public:
		Material(std::string_view path, std::string_view name);
		virtual ~Material();

		static cgc::strong_ptr<Material> Construct(const std::string& path, TEXTURE_FILTER textureFlter = TEXTURE_FILTER::NEAREST);

		virtual void Bind();

		virtual cgc::strong_ptr<IShader> GetShader() const = 0;

		void SetBuffer(hash_t bindHash, const cgc::strong_ptr<Buffer>& buffer);
		void SetTexture1D(hash_t bindHash, const cgc::strong_ptr<Texture1D>& texture);
		void SetTexture2D(hash_t bindHash, const cgc::strong_ptr<Texture2D>& texture);

		inline const std::string& GetPath() const { return path; }
		inline const std::string& GetName() const { return name; }

		inline const Buffers::pair<Buffer>* GetBufferBegin() { return reinterpret_cast<const Buffers::pair<Buffer>*>(buffers.data.data()); }
		inline const Buffers::pair<Buffer>* GetBufferEnd() { return reinterpret_cast<const Buffers::pair<Buffer>*>(buffers.data.data() + buffers.bufferSize); }

		inline const Buffers::pair<Texture1D>* GetTexture1DBegin() { return reinterpret_cast<const Buffers::pair<Texture1D>*>(buffers.data.data() + buffers.bufferSize); }
		inline const Buffers::pair<Texture1D>* GetTexture1DEnd() { return reinterpret_cast<const Buffers::pair<Texture1D>*>(buffers.data.data() + buffers.texture1DSize); }

		inline const Buffers::pair<Texture2D>* GetTexture2DBegin() { return reinterpret_cast<const Buffers::pair<Texture2D>*>(buffers.data.data() + buffers.texture1DSize); }
		inline const Buffers::pair<Texture2D>* GetTexture2DEnd() { return reinterpret_cast<const Buffers::pair<Texture2D>*>(buffers.data.data() + buffers.texture2DSize); }

		inline bool IsTranslucent() const { return transparent; }
		inline bool IsDoubleSided() const { return doubleSided; }
		inline bool GetReflective() const { return reflective; }
	};
}